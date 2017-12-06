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

		enum Header
		{
			ContentType,
			ContentLength,

			HeaderMax
		};
		static const char* HeaderStr(Header h)
		{
			static const char* const str[] =
			{
				"Content-Type",
				"Content-Length"
			};
			return str[int(h)];
		}
		static const char* ContentTypeJson = "application/hap+json";
		static const char* ContentTypeTlv8 = "application/pairing+tlv8";

		// Http request parser
		template<int MaxHeaders>
		class Parser
		{
		private:
			char *_buf;
			uint16_t _size;
			struct phr_header _headers[MaxHeaders];
			const char *_method;
			const char *_path;
			const uint8_t* _data;
			size_t _method_len;
			size_t _path_len;
			size_t _data_len;
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

			Parser(char *buf, uint16_t size)
				: _buf(buf), _size(size)
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

			char* buf()
			{
				return _buf;
			}

			uint16_t size()
			{
				return _size;
			}

			// parse buffer, maybe called multiple times as more data is read into the buffer
			//	the buflen must indicate current length of valid data in the buffer
			Status parse(size_t buflen)
			{
				_prevbuflen = _buflen;
				_buflen = buflen;
				_num_headers = sizeofarr(_headers);

				int rc = phr_parse_request(_buf, _buflen, 
					&_method, &_method_len, &_path, &_path_len,
					&_minor_version, _headers, &_num_headers, _prevbuflen);

				if (rc > 0)
				{
					_data = (uint8_t*)_buf + rc;
					_data_len = _buflen - rc;
					return Success;
				}
				
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

			auto data()
			{
				return std::make_pair(_data, _data_len);
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

			// return true if header h exists, and value of integer parameter
			bool hdr(Header h, int& prm)
			{
				for (size_t i = 0; i < _num_headers; i++)
				{
					const char* s = HeaderStr(h);

					if (strlen(s) == _headers[i].name_len && 
						memcmp(_headers[i].name, s, _headers[i].name_len) == 0)
					{
						char v[16];
						if (_headers[i].value_len < sizeof(v))
						{
							memcpy(v, _headers[i].value, _headers[i].value_len);
							v[_headers[i].value_len] = 0;

							prm = atoi(v);
							return true;
						}
						else
						{
							Log("Http: %s is too big\n", s);
						}
					}
				}
				return false;
			}

			// returns true if header h exists and its value matches prm
			bool hdr(Header h, const char* prm)
			{
				for (size_t i = 0; i < _num_headers; i++)
				{
					const char* s = HeaderStr(h);

					if (strlen(s) == _headers[i].name_len &&
						memcmp(_headers[i].name, s, _headers[i].name_len) == 0)
					{
						int l = strlen(prm);
						return l == _headers[i].value_len &&
							memcmp(prm, _headers[i].value, l) == 0;
					}
				}
				return false;
			}

		};

		// HTTP response creator
		class Response
		{
		private:
			char* _buf;
			uint16_t _max = 0;
			uint16_t _len = 0;
			uint16_t _len_pos = 0;

		public:
			Response(char* buf, uint16_t size)
				: _buf(buf), _max(size)
			{}

			// return response buffer
			char* buf()
			{
				if (_max == 0)
					return nullptr;
				return _buf;
			}

			// return length of valid data in the buffer
			uint16_t len()
			{
				return _len;
			}

			// return pointer to data area after headers
			char* data()
			{
				return _buf + _len;
			}

			// return size of data area
			uint16_t size()
			{
				return _max - _len;
			}

			bool start(Status status)
			{
				_len = snprintf(_buf, _max, "HTTP/1.1 %s\r\n", StatusStr(status));
				_max -= _len;
				return _max != 0;
			}

			// add header with integer parameter
			bool add(Header h, int prm)
			{
				if (_max == 0)
					return false;

				int l = 0;

				l = snprintf(_buf + _len, _max, "%s: %4d\r\n", HeaderStr(h), prm);
				_len += l;
				_max -= _len;

				if (h == ContentLength)
				{
					_len_pos = _len - 2;
				}

				return _max != 0;
			}

			// add length of data area
			//	assumes that _len_pos was saved by prevous call to add(ConteneLength,0)
			void setContentLength(uint16_t len)
			{
				if (_len_pos == 0)
					return;

				char t = _buf[_len_pos];
				snprintf(_buf + _len_pos - 4, 5, "%4d", len);
				_buf[_len_pos] = t;

				_len += len;
			}

			bool add(Header h, const char* prm)
			{
				if (_max == 0)
					return false;

				int l = 0;
				l = snprintf(_buf + _len, _max, "%s: %s\r\n", HeaderStr(h), prm);
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
		//	- all access to Http Server object must be externally serialized
		class Server
		{
		private:
			Db& _db;						// accessory database
			Pairings& _pairings;			// pairings database
			Hap::Crypt::Ed25519& _keys;		// crypto keys
			
			class Session					// sessions
			{
			public:
				// the following fields are valid during one HTTP request/response exchange
				Parser<MaxHttpHeaders> req;			// HTTP request
				Response rsp;						// HTTP response
				Hap::Tlv::Parse<MaxHttpTlv> tlvi;	// incoming TLV parser
				Hap::Tlv::Create tlvo;				// outgoing TLV creator
				
				// session-wide data
				Hap::Crypt::Curve25519 curve;		// Session securiry keys (used on Pair Verivication phase)
				const Controller* ios;				// paired iOS device - if not NULL then the session is secured
				uint8_t AccessoryToControllerKey[32];
				uint8_t ControllerToAccessoryKey[32];
				
				// session temp data
				uint8_t key[32];
				uint8_t data[512];

				// session constructor, executed once during server object initialization
				Session()
					: req(req_b, sizeof(req_b)), rsp(rsp_b, sizeof(rsp_b))
				{}

				void Open(sid_t sid)
				{
					_sid = sid;
					opened = true;
					ios = nullptr;
				}

				void Close()
				{
					opened = false;
					_sid = sid_invalid;
					ios = nullptr;
				}

				bool isOpen()
				{
					return opened;
				}

				sid_t Sid()
				{
					if (opened)
						return _sid;
					return sid_invalid;
				}

			private:
				// the following fields are valid from session open to close
				bool opened = false;		// true when session is opened
				sid_t _sid = sid_invalid;	// valid when opened

				char req_b[MaxHttpFrame];	// request buffer
				char rsp_b[MaxHttpFrame];	// response buffer
			} _sess[MaxHttpSessions + 1];	// last slot is for handling 'too many sessions' condition

		public:
			Server(Db& db, Pairings& pairings, Hap::Crypt::Ed25519& keys)
				: _db(db), _pairings(pairings), _keys(keys)
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

		private:
			void PairSetup_M1(Session* sess);
			void PairSetup_M3(Session* sess);
			void PairSetup_M5(Session* sess);
			void PairVerify_M1(Session* sess);
			void PairVerify_M3(Session* sess);
		};
	}
}

#endif