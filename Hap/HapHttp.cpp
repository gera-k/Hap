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
			std::function<int(sid_t sid, void* ctx, char* buf, uint16_t size)> recv,
			std::function<int(sid_t sid, void* ctx, char* buf, uint16_t len)> send
		)
		{
			if (sid > MaxHttpSessions)	// invalid sid
				return false;

			Session* sess = &_sess[sid];
			Response rsp(sess->rsp, sizeof(sess->rsp));

			if (sid == MaxHttpSessions)	// too many sessions
			{
				// TODO: read request, create error response
				send(sid, ctx, rsp.buf(), rsp.len());
				return false;
			}

			_reqp.init();

			// read and parse the HTTP request
			uint16_t len = 0;		// total len of valid data in req
			while (true)
			{
				char* req = sess->req + len;
				uint16_t req_len = sizeof(sess->req) - len;
				
				// read next portion of the request
				int l = recv(sid, ctx, req, req_len);
				if (l < 0)	// read error
				{
					Log("Http: Read Error\n");
					return false;
				}
				if (l == 0)
				{
					Log("Http: Read EOF\n");
					return false;
				}

				len += req_len;

				// TODO: it session is secured, decode the next portion, if whole block is received

				// parse HTTP request
				auto status = _reqp.parse(sess->req, len);
				if (status == _reqp.Error)	// parser error
				{
					// TODO: make response Internal server error
					send(sid, ctx, rsp.buf(), rsp.len());
					return false;
				}

				if (status == _reqp.Success)
					break;

				// request incomplete - try reading more data
			}

			auto m = _reqp.method();
			Log("Method: '%.*s'\n", m.second, m.first);

			auto p = _reqp.path();
			Log("Path: '%.*s'\n", p.second, p.first);

			for (size_t i = 0; i < _reqp.hdr_count(); i++)
			{
				auto n = _reqp.hdr_name(i);
				auto v = _reqp.hdr_value(i);
				Log("%.*s: '%.*s'\n", n.second, n.first, v.second, v.first);
			}

			if (m.second == 4 && strncmp(m.first, "POST", 4) == 0)
			{
				// POST
				//		/identify
				//		/pair-setup
				//		/pair-verify
				//		/pairings

				if (p.second == 9 && strncmp(p.first, "/identify", 9) == 0)
				{
					if (_pairings.Count() == 0)
					{
						Log("Http: Exec unpaired identify\n");
						rsp.start(HTTP_204);
						rsp.end();
					}
					else
					{
						Log("Http: Unpaired identify prohibited when paired\n");
						rsp.start(HTTP_400);
						rsp.add(ContentTypeJson);
						rsp.end("{\"status\":-70401}");
					}
				}
			}
			else if (m.second == 3 && strncmp(m.first, "GET", 3) == 0)
			{
				// GET
				//		/accessories
				//		/characteristics
			}
			else if (m.second == 3 && strncmp(m.first, "PUT", 3) == 0)
			{
				// PUT
				//		/characteristics
			}

			send(sid, ctx, rsp.buf(), rsp.len());
			return true;
		}


	}
}
