
#include "ConnectionID.hpp"

ConnectionID::ConnectionID() { SetDefaults("Null"); }
ConnectionID::ConnectionID(const char *name) { SetDefaults(name); }

void ConnectionID::SetDefaults(const char *name) {
    Name=name;
    AddressLabel="Null";
    Address=0;
    Port=0;
    Rate=0;
	Type="notype";

    ConnectionEnable=false;

    Host="localhost";
    UserName="admin";
    Pswd="admin";
    Description="...";

	ID=0;
    IOMapID=0;
}

void ConnectionID::FormatNameType() {

	for (auto &c: Name) c = toupper(c);
	for (auto &c: Type) c = toupper(c);
}

#if 0
ConnectionID& ConnectionID::operator () (unsigned id, const char *name) {
    ID=id;
    Name=name;
    return(*this);
}
#endif

//}
