#include "Hap.h"

#include "srp\srp.h"

namespace Hap
{
	namespace Http
	{
		SRP* srp = nullptr;

		// Open
		//	returns new session ID, 0..sid_max, or sid_invalid
		sid_t Server::Open()
		{
			for (sid_t sid = 0; sid < sizeofarr(_sess); sid++)
			{
				if (_sess[sid].isOpen())
					continue;

				_sess[sid].Open();

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

			if (!_sess[sid].isOpen())
				return false;

			_db.Close(sid);

			_sess[sid].Close();

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

			if (sid == MaxHttpSessions)	// too many sessions
			{
				// TODO: read request, create error response
				send(sid, ctx, sess->rsp.buf(), sess->rsp.len());
				return false;
			}

			sess->req.init();

			// read and parse the HTTP request
			uint16_t len = 0;		// total len of valid data in req
			while (true)
			{
				char* req = sess->req.buf() + len;
				uint16_t req_len = sess->req.size() - len;
				
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

				len += l;

				// TODO: it session is secured, decode the next portion, if whole block is received

				// parse HTTP request
				auto status = sess->req.parse(len);
				if (status == sess->req.Error)	// parser error
				{
					// TODO: make response Internal server error
					send(sid, ctx, sess->rsp.buf(), sess->rsp.len());
					return false;
				}

				if (status == sess->req.Success)
					break;

				// request incomplete - try reading more data
			}

			auto m = sess->req.method();
			Log("Method: '%.*s'\n", m.second, m.first);

			auto p = sess->req.path();
			Log("Path: '%.*s'\n", p.second, p.first);

			auto d = sess->req.data();

			for (size_t i = 0; i < sess->req.hdr_count(); i++)
			{
				auto n = sess->req.hdr_name(i);
				auto v = sess->req.hdr_value(i);
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
					if (0 && _pairings.Count() == 0)
					{
						Log("Http: Exec unpaired identify\n");
						sess->rsp.start(HTTP_204);
						sess->rsp.end();
					}
					else
					{
						Log("Http: Unpaired identify prohibited when paired\n");
						sess->rsp.start(HTTP_400);
						sess->rsp.add(ContentType, ContentTypeJson);
						sess->rsp.end("{\"status\":-70401}");
					}
				}
				else if (p.second == 11 && strncmp(p.first, "/pair-setup", 11) == 0)
				{
					int len;
					if (!sess->req.hdr(ContentType, ContentTypeTlv8))
					{
						Log("Http: Unknown or missing ContentType\n");
						sess->rsp.start(HTTP_400);
						sess->rsp.end();
					}
					else if (!sess->req.hdr(ContentLength, len))
					{
						Log("Http: Unknown or missing ContentLength\n");
						sess->rsp.start(HTTP_400);
						sess->rsp.end();
					}
					else
					{
						sess->tlvi.parse(d.first, d.second);
						Log("PairSetup: TLV item count %d\n", sess->tlvi.count());

						Tlv::Method method;
						Tlv::State state;
						if (!sess->tlvi.get(Tlv::Type::Method, method))
						{
							Log("PairSetup: Method not found\n");
						}
						else if (method != Tlv::Method::PairSetup)
						{
							Log("PairSetup: Invalid Method\n");
						}
						else if (!sess->tlvi.get(Tlv::Type::State, state))
						{
							Log("PairSetup: State not found\n");
						}
						else
						{
							switch (state)
							{
							case Tlv::State::M1:
								PairSetup_M1(sess);
								break;

							default:
								Log("PairSetup: Unknown state %d\n", (int)state);
							}
						}
					}
				}
				else if (p.second == 12 && strncmp(p.first, "/pair-verify", 12) == 0)
				{

				}
				else if (p.second == 9 && strncmp(p.first, "/pairings", 9) == 0)
				{

				}
				else
				{
					Log("Http: Unknown path %.*s\n", p.second, p.first);
					sess->rsp.start(HTTP_400);
					sess->rsp.end();
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

			send(sid, ctx, sess->rsp.buf(), sess->rsp.len());
			return true;
		}

		void Server::PairSetup_M1(Session* sess)
		{
			Log("PairSetup M1\n");

			srp = SRP_new(SRP6a_server_method());

			// first create headers without data
			sess->rsp.start(HTTP_200);
			sess->rsp.add(ContentType, ContentTypeTlv8);
			sess->rsp.add(ContentLength, 0);
			sess->rsp.end();
			
			// create response TLV in the response buffer right after HTTP headers 
			sess->tlvo.create(sess->rsp.data(), sess->rsp.size());
			sess->tlvo.add(Hap::Tlv::Type::State, Hap::Tlv::State::M2);
			sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::Unavailable);

			// adjust content length
			sess->rsp.setContentLength(sess->tlvo.length());
		}
	}
}
