#ifndef _HAP_HTTP_H_
#define _HAP_HTTP_H_

#include "picohttpparser.h"

namespace Hap
{
	namespace Http
	{
		enum Status
		{
			HTTP_200,
			HTTP_204,
			HTTP_207,
			HTTP_400,
			HTTP_404,
			HTTP_405,
			HTTP_422,
			HTTP_429,
			HTTP_470,
			HTTP_500,
			HTTP_503
		};
		static const char* StatusStr(Status c)
		{
			static const char* const str[] =
			{
				"200 OK",
				"204 No Content",
				"207 Multi-Status",
				"400 Bad Request",
				"404 Not Found",
				"405 Method Not Allowed",
				"422 Unprocessable Entry",
				"429 Too Many requests",
				"470 Connection Authorization Required",
				"500 Internal Server Error",
				"503 Service Unavailable",
			};
			return str[int(c)];
		}

		// Http request parser
		template<int MaxHeaders>
		class Parser
		{
		private:
			struct phr_header _headers[MaxHeaders];
			const char *_method;
			const char *_path;
			size_t _method_len;
			size_t _path_len;
			size_t _num_headers;
			size_t _buflen;
			size_t _prevbuflen;
			int _minor_version;

		public:
			enum Status
			{
				Error = -1,
				Success = 0,
				Incomplete = 1,
			};

			Parser()
			{
			}

			void init()
			{
				_method_len = 0;
				_path_len = 0;
				_num_headers = 0;
				_buflen = 0;
				_prevbuflen = 0;
			}

			// parse buffer, maybe called multiple times as more data is read into the buffer
			//	the buflen must indicate current length of valid data in the buffer
			Status parse(const char* buf, size_t buflen)
			{
				_prevbuflen = _buflen;
				_buflen = buflen;
				_num_headers = sizeofarr(_headers);

				int rc = phr_parse_request(buf, _buflen, 
					&_method, &_method_len, &_path, &_path_len,
					&_minor_version, _headers, &_num_headers, _prevbuflen);

				if (rc > 0)
					return Success;
				
				if (rc == -1)
					return Error;

				return Incomplete;
			}

			auto method()
			{
				return std::make_pair(_method, _method_len);
			}

			auto path()
			{
				return std::make_pair(_path, _path_len);
			}

			size_t hdr_count()
			{
				return _num_headers;
			}

			auto hdr_name(size_t i)
			{
				if (i < _num_headers)
					return std::make_pair(_headers[i].name, _headers[i].name_len);
				return std::make_pair((const char *)0, (size_t)0);
			}

			auto hdr_value(size_t i)
			{
				if (i < _num_headers)
					return std::make_pair(_headers[i].value, _headers[i].value_len);
				return std::make_pair((const char *)0, (size_t)0);
			}

		};

		enum Header
		{
			ContentTypeJson,
			ContentLength,
		};
		static const char* HeaderStr(Header h)
		{
			static const char* const str[] =
			{
				"Content-Type: application/hap+json",
				"Content-Length: "
			};
			return str[int(h)];
		}

		// HTTP response creator
		class Response
		{
		private:
			char* _buf;
			uint16_t _max = 0;
			uint16_t _len = 0;

		public:
			Response(char* buf, uint16_t size)
				: _buf(buf), _max(size)
			{}

			char* buf()
			{
				if (_max == 0)
					return nullptr;
				return _buf;
			}

			uint16_t len()
			{
				return _len;
			}

			bool start(Status status)
			{
				_len = snprintf(_buf, _max, "HTTP/1.1 %s\r\n", StatusStr(status));
				_max -= _len;
				return _max != 0;
			}

			bool add(Header h, int prm = 0)
			{
				if (_max == 0)
					return false;

				int l = 0;
				switch (h)
				{
				case ContentTypeJson:
					l = snprintf(_buf + _len, _max, "%s\r\n", HeaderStr(h));
					break;
				case ContentLength:
					l = snprintf(_buf + _len, _max, "%s%d\r\n", HeaderStr(h), prm);
					break;
				}
				_len += l;
				_max -= _len;
				return _max != 0;
			}

			// end HTTP response with no data
			bool end()
			{
				if (_max == 0)
					return false;

				int l = snprintf(_buf + _len, _max, "\r\n");
				_len += l;
				_max -= l;
				return _max != 0;
			}

			// end HTTP response, attach data from string
			bool end(const char* s, int l = 0)
			{
				if (_max == 0)
					return false;

				if (l == 0)
					l = strlen(s);
				if (!add(ContentLength, l))
					return false;
				l = snprintf(_buf + _len, _max, "\r\n%s", s);
				_len += l;
				_max -= l;
				return _max != 0;
			}
		};

		// Http Server object
		//	- all access to Http object must be externally serialized
		class Server
		{
		private:
			Db& _db;						// accessory database
			Pairings& _pairings;			// pairings database
			Parser<MaxHttpHeaders> _reqp;	// HTTP request parser
			struct Session					// sessions
			{
				bool opened;				// true when session is opened
				char req[MaxHttpFrame];		// request buffer
				char rsp[MaxHttpFrame];		// response buffer
			} _sess[MaxHttpSessions + 1];	// last slot is for handling 'too many sessions' condition

		public:
			Server(Db& db, Pairings& pairings)
				: _db(db), _pairings(pairings)
			{}

			// Open - returns new session ID, 0..sid_max, or sid_invalid
			//	the caller (network task) calls Open when new TCP connection request arrives
			//	when sid_invalid is returned, the caller should still call Process
			//	which will create and send correct error response (503 Unavailable or 
			//	429 Too many requests)
			sid_t Open();

			// Close - returns true if opened session was closed
			//	the caller (network task) must call Close when TCP connection associated with 
			//	this session is disconnected
			bool Close(sid_t sid);

			// Process - process incoming HTTP request
			//	must be called from network task when data for this session is available
			//	the function: 
			//		- calls 'recv' one or more times until complete HTTP request is arrived
			//			when recv error or data timeout is detected the caller returns false 
			//			and disconnects the TCP session 
			//		- processes the request and creates response
			//		- calls 'send' to send the response back
			//			buf is send to nullptr if response buffer is too small
			//		-	returns true after calling 'send', no matter if there was HTTP error or not
			//		-	returns false the caller close the TCP session
			bool Process(
				sid_t sid,		// session ID returned from Open
				void* ctx,		// caller context, passed back in recv and send callbacks
				std::function<int(sid_t sid, void* ctx, char* buf, uint16_t size)> recv,
				std::function<int(sid_t sid, void* ctx, char* buf, uint16_t len)> send
			);
		};
	}
}

#endif