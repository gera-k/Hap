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
			Status parse(const uint8_t* buf, size_t buflen)
			{
				_prevbuflen = _buflen;
				_buflen = buflen;
				_num_headers = sizeofarr(_headers);

				int rc = phr_parse_request(reinterpret_cast<const char*>(buf), _buflen, 
					&_method, &_method_len, &_path, &_path_len,
					&_minor_version, _headers, &_num_headers, _prevbuflen);

				if (rc > 0)
					return Success;
				
				if (rc == -1)
					return Error;

				return Incomplete;
			}


		};

		// Http Server object
		//	- all access to Http object must be externally serialized
		class Server
		{
		private:
			Db& _db;						// accessory database
			Parser<MaxHttpHeaders> _reqp;	// HTTP request parser
			struct Session					// sessions
			{
				bool opened;				// true when session is opened
				uint8_t req[MaxHttpFrame];	// request buffer
				uint8_t rsp[MaxHttpFrame];	// response buffer
			} _sess[MaxHttpSessions + 1];	// last slot is for handling 'too many sessions' condition

		public:
			Server(Db& db)
				: _db(db)
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
			//		- calls 'send' to send the response back, close flag is set when the TCP session 
			//			must be disconnected after sending the response
			//		-	returns true after calling 'send', no matter if there was HTTP error or not
			//		-	returns false the caller close the TCP session
			bool Process(
				sid_t sid,		// session ID returned from Open
				void* ctx,		// caller context, passed back in recv and send callbacks
				std::function<bool(sid_t sid, void* ctx, uint8_t* buf, uint16_t& size)> recv,
				std::function<bool(sid_t sid, void* ctx, uint8_t* buf, uint16_t len)> send
			);
		};
	}
}

#endif