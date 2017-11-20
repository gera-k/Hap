#include "Hap.h"

namespace Hap
{
	namespace Http
	{

		// Open
		//	returns new session ID, 0..sid_max, or sid_invalid
		sid_t Server::Open()
		{
			for (sid_t sid = 0; sid < sizeofarr(_sess); sid++)
			{
				if (_sess[sid].opened)
					continue;

				_sess[sid].opened = true;

				// open database
				_db.Open(sid);

				return sid;
			}

			return sid_invalid;
		}

		// Close
		//	returns true if opened session was closed
		bool Server::Close(sid_t sid)
		{
			if (sid > sid_max)
				return false;

			if (!_sess[sid].opened)
				return false;

			_sess[sid].opened = false;

			_db.Close(sid);

			return true;
		}

		bool Server::Process(
			sid_t sid,
			void* ctx,
			std::function<bool(sid_t sid, void* ctx, uint8_t* buf, uint16_t& size)> recv,
			std::function<bool(sid_t sid, void* ctx, uint8_t* buf, uint16_t len, bool close)> send
		)
		{
			if (sid > MaxHttpSessions)	// invalid sid
				return false;

			Session* sess = &_sess[sid];
			uint16_t rsp_len = 0;

			if (sid == MaxHttpSessions)	// too many sessions
			{
				// TODO: read request, create error response
				send(sid, ctx, sess->rsp, rsp_len, true);
				return true;
			}

			_reqp.init();

			uint16_t len = 0;		// total len of valid data in req
			while (true)
			{
				uint8_t* req = sess->req + len;
				uint16_t req_len = sizeof(sess->req) - len;
				
				// read next portion of the request
				if (!recv(sid, ctx, req, req_len))	// read error
				{
					// TODO: make response Internal server error
					send(sid, ctx, sess->rsp, rsp_len, true);
					return true;
				}

				len += req_len;

				// parse HTTP request
				auto status = _reqp.parse(sess->req, len);
				if (status == _reqp.Error)	// parser error
				{
					// TODO: make response Internal server error
					send(sid, ctx, sess->rsp, rsp_len, true);
					return true;
				}

				if (status == _reqp.Success)
					break;

				// request incomplete - try reading more data
			}

			send(sid, ctx, sess->rsp, rsp_len, true);
			return true;
		}


	}
}
