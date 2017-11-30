#include "Hap.h"

#include "srp\srp.h"

namespace Hap
{
	namespace Http
	{
		const unsigned char modulus[] = { 
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34, 
			0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 
			0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 
			0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 
			0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD, 
			0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 
			0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37, 
			0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 
			0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 
			0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 
			0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED, 
			0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 
			0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6, 
			0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 
			0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 
			0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 
			0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F, 
			0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 
			0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB, 
			0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 
			0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 
			0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 
			0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B, 
			0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 
			0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F, 
			0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 
			0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 
			0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 
			0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10, 
			0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D, 
			0xAD, 0x33, 0x17, 0x0D, 0x04, 0x50, 0x7A, 0x33, 
			0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64, 
			0xEC, 0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A, 
			0x8A, 0xEA, 0x71, 0x57, 0x5D, 0x06, 0x0C, 0x7D, 
			0xB3, 0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7, 
			0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7, 
			0x1E, 0x8C, 0x94, 0xE0, 0x4A, 0x25, 0x61, 0x9D, 
			0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B, 
			0xF1, 0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64, 
			0xD8, 0x76, 0x02, 0x73, 0x3E, 0xC8, 0x6A, 0x64, 
			0x52, 0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C, 
			0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C, 
			0x77, 0x09, 0x88, 0xC0, 0xBA, 0xD9, 0x46, 0xE2, 
			0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31, 
			0x43, 0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E, 
			0x4B, 0x82, 0xD1, 0x20, 0xA9, 0x3A, 0xD2, 0xCA, 
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		const unsigned char generator[] = { 0x05 };

		// current pairing
		SRP* srp = NULL;					// !NULL = pairing in progress, only one pairing at a time
		uint8_t srp_pub_key[384];			// pairing iOS device public key
		uint8_t srp_proof[64];				// pairing iOS device proof
		sid_t srp_owner = sid_invalid;		// session owning the srp
		uint8_t srp_auth_count = 0;			// auth attempts counter

		// Open
		//	returns new session ID, 0..sid_max, or sid_invalid
		sid_t Server::Open()
		{
			for (sid_t sid = 0; sid < sizeofarr(_sess); sid++)
			{
				if (_sess[sid].isOpen())
					continue;

				_sess[sid].Open(sid);

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

			// cancel current pairing if any
			if (srp != NULL && srp_owner == sid)
			{
				SRP_free(srp);
				srp = NULL;
				srp_owner = sid_invalid;
			}

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

						Tlv::State state;
						if (!sess->tlvi.get(Tlv::Type::State, state))
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

							case Tlv::State::M3:
								PairSetup_M3(sess);
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
			cstr* pub = NULL;

			Log("PairSetupM1\n");

			// prepare response without data
			sess->rsp.start(HTTP_200);
			sess->rsp.add(ContentType, ContentTypeTlv8);
			sess->rsp.add(ContentLength, 0);
			sess->rsp.end();

			// create response TLV in the response buffer right after HTTP headers 
			sess->tlvo.create(sess->rsp.data(), sess->rsp.size());
			sess->tlvo.add(Hap::Tlv::Type::State, Hap::Tlv::State::M2);

			// verify that valid Method is present in input TLV
			Tlv::Method method;
			if (!sess->tlvi.get(Tlv::Type::Method, method))
			{
				Log("PairSetupM1: Method not found\n");
				goto RetErr;
			}
			if (method != Tlv::Method::PairSetup)
			{
				Log("PairSetupM1: Invalid Method\n");
				goto RetErr;
			}

			// if the accessory is already paired it must respond Error_Unavailable
			if (_pairings.Count() != 0)
			{
				Log("PairSetupM1: Already paired, return Error_Unavailable\n");
				sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::Unavailable);
				goto Ret;
			}

			// if accessory received more than 100 unsuccessfull auth attempts, respond Error_MaxTries
			if (srp_auth_count > 100)
			{
				Log("PairSetupM1: Too many auth attempts, return Error_MaxTries\n");
				sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::MaxTries);
				goto Ret;
			}

			// if accessory is currently performing PairSetup with different controller, respond Error_Busy
			if (srp != NULL && srp_owner != sess->Sid())
			{
				Log("PairSetupM1: Already pairing, return Error_Busy\n");
				sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::Busy);
				goto Ret;
			}
			
			// create new pairing session
			srp = SRP_new(SRP6a_server_method());
			if (srp == NULL)
			{
				Log("PairSetupM1: SRP_new error\n");
				goto RetErr;
			}

			srp_owner = sess->Sid();
			srp_auth_count++;

			int rc = SRP_set_username(srp, "Pair-Setup");
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM1: SRP_set_username error %d\n", rc);
				goto RetErr;
			}

			char salt[16];
			t_random(salt, 16);
			rc = SRP_set_params(srp,
				modulus, sizeof(modulus),
				generator, sizeof(generator),
				(uint8_t*)salt, 16
			);
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM1: SRP_set_params error %d\n", rc);
				goto RetErr;
			}

			char pwd[] = "000-11-000";				// TODO: get from sequred storage
			rc = SRP_set_auth_password(srp, pwd);
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM1: SRP_set_auth_password error %d\n", rc);
				goto RetErr;
			}

			rc = SRP_gen_pub(srp, &pub);
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM1: SRP_gen_pub error %d\n", rc);
				goto RetErr;
			}

			sess->tlvo.add(Hap::Tlv::Type::PublicKey, (uint8_t*)pub->data, (uint16_t)pub->length);
			sess->tlvo.add(Hap::Tlv::Type::Salt, (uint8_t*)salt, 16);

			goto Ret;

		RetErr:
			if (srp)
				SRP_free(srp);
			srp = NULL;
			srp_owner = sid_invalid;
			sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::Unknown);
		
		Ret:
			if(pub != NULL)
				cstr_free(pub);

			// adjust content length in response
			sess->rsp.setContentLength(sess->tlvo.length());
		}
	
		void Server::PairSetup_M3(Session* sess)
		{
			cstr* key = NULL;
			cstr* rsp = NULL;

			Log("PairSetupM3\n");

			// prepare response without data
			sess->rsp.start(HTTP_200);
			sess->rsp.add(ContentType, ContentTypeTlv8);
			sess->rsp.add(ContentLength, 0);
			sess->rsp.end();

			// create response TLV in the response buffer right after HTTP headers 
			sess->tlvo.create(sess->rsp.data(), sess->rsp.size());
			sess->tlvo.add(Hap::Tlv::Type::State, Hap::Tlv::State::M4);

			// verify that pairing is in progress on current session
			if (srp == nullptr || srp_owner != sess->Sid())
			{
				Log("PairSetupM3: No active pairing\n");
				goto RetErr;
			}

			// verify that required items are present in input TLV
			uint16_t size = sizeof(srp_pub_key);
			if (!sess->tlvi.get(Tlv::Type::PublicKey, srp_pub_key, size))
			{
				Log("PairSetupM3: PublicKey not found\n");
				goto RetErr;
			}
			size = sizeof(srp_proof);
			if (!sess->tlvi.get(Tlv::Type::Proof, srp_proof, size))
			{
				Log("PairSetupM3: Proof not found\n");
				goto RetErr;
			}

			int rc = SRP_compute_key(srp, &key, srp_pub_key, sizeof(srp_pub_key));
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM3: SRP_compute_key error %d\n", rc);
				goto RetErr;
			}

			rc = SRP_verify(srp, srp_proof, size);
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM3: SRP_verify error %d\n", rc);
				sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::Authentication);
				goto Ret;
			}

			rc = SRP_respond(srp, &rsp);
			if (rc != SRP_SUCCESS)
			{
				Log("PairSetupM3: SRP_respond error %d\n", rc);
				goto RetErr;
			}

			sess->tlvo.add(Hap::Tlv::Type::Proof, (uint8_t*)rsp->data, (uint16_t)rsp->length);

			goto Ret;

		RetErr:	// error, cancel current pairing, if this session owns it
			if (srp && srp_owner == sess->Sid())
			{
				SRP_free(srp);
				srp = NULL;
				srp_owner = sid_invalid;
			}
			sess->tlvo.add(Hap::Tlv::Type::Error, Hap::Tlv::Error::Unknown);

		Ret:
			if (key != NULL)
				cstr_free(key);
			if (rsp != NULL)
				cstr_free(rsp);

			// adjust content length in response
			sess->rsp.setContentLength(sess->tlvo.length());
		}

	}
}
