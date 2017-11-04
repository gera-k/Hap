#include <SDKDDKVer.h>

#include <tchar.h>
#include <string>

#include "Hap.h"

//Hap::Service::Lightbulb lb(5);

class MyAcc : public Hap::Accessory<1>
{
private:
	Hap::Lightbulb lb = 6;
public:
	MyAcc(Hap::Property::AccessoryInstanceId::T aid = 0) : Hap::Accessory<1>(aid)
	{
		AddServ(&lb);
	}
};

class MyDb : public Hap::Db<1>
{
private:
	MyAcc acc;
public:
	MyDb()
	{
		acc.aid(1);
		AddAcc(&acc);
	}

	void test()
	{
		auto a = GetAcc(1);
		auto b = GetAcc(2);

		printf("acc %p  a %p  b %p\n", &acc, a, b);
	}
} db;

int main()
{
	char str[256];
	Hap::Property::Type ty("asdfgh");
	Hap::Property::InstanceId iid;
	Hap::Property::EventNotifications en(false);
	Hap::Property::LinkedServices<5> ls;
	Hap::Property::MinimumValue<Hap::FormatId::Int> mvi(10);
	Hap::Property::MinimumValue<Hap::FormatId::Float> mvf(1.1);
	int l;

//	printf("sizeof(c1)=%d  type '%s'  iid %lld\n",
//		sizeof(c1), c1.Type(), c1.Iid());

//	printf("sizeof(c2)=%d  type '%s'  iid %lld\n",
//		sizeof(c2), c2.Type(), c2.Iid());

//	Hap::Characteristic::On on(3);
//	on.on();
//	int l = on.getDb(str, sizeof(str) - 1);
//	str[l] = 0;
	
//	printf("sizeof(on)=%d  type '%s'  iid %lld  val %d  db '%s'\n",
//		sizeof(on), on.Type(), on.Iid(), on.Value(), str);

//	lb.On().on();

//	l = lb.getDb(str, sizeof(str) - 1);
//	str[l] = 0;
//	printf("sizeof(lb)=%d  type '%s'  iid %lld  db '%s'\n",
//		sizeof(lb), lb.Type().get(), lb.Iid().get(), str);

	l = db.getDb(str, sizeof(str) - 1);
	str[l] = 0;
	printf("sizeof(srv)=%d  db '%s'\n",
		sizeof(db), str);

	db.test();

//	Hap::Accessory<Hap::Property::Type, Hap::Property::InstanceId> acc(ty, iid);

//	printf("ty=%s  sizeof(ty)=%d FormatId %d  Size %d  Length %d  Format '%s'  Key '%s'\n",
//		ty.get(), sizeof(ty), ty.formatId(), ty.size(), ty.length(), ty.format(), ty.key());

//	printf("en=%d  sizeof(en)=%d FormatId %d  Size %d  Length %d  Format '%s'  Key '%s'\n",
//		en.get(), sizeof(en), en.formatId(), en.size(), en.length(), en.format(), en.key());
	en.set(true);
	printf("en=%d\n", en.get());

	Hap::Property::Permissions pm(Hap::Property::Permissions::PairedRead | Hap::Property::Permissions::PairedWrite);
	printf("isPairedWrite: %d  isEvent: %d\n",
		pm.isEnabled(Hap::Property::Permissions::PairedWrite),
		pm.isEnabled(Hap::Property::Permissions::Events)
	);

//	ls[1] = 1234;
	ls.set(2, 3456);
//	printf("sizeof(ls)=%d FormatId %d  Size %d  Length %d  ls[0] %lld  ls[1] %lld  ls[2] %lld\n",
//		sizeof(ls), ls.formatId(), ls.size(), ls.length(), ls[0], ls[1], ls.get(2));

	printf("mvi %d  mvf %g\n", mvi.get(), mvf.get());

	const char req[] = "{\"characteristics\":[{\"aid\":1,\"iid\":8,\"value\":true},{\"aid\":3,\"iid\":8,\"ev\":true}]}";
	char rsp[128];

	auto rc = db.Write(req, sizeof(req), rsp, sizeof(rsp));
	Log("Write: %d\n", rc);

	return 0;
}

