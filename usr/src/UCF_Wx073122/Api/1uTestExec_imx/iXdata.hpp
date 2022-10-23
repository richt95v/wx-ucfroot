
#ifndef IXDATA
#define IXDATA

class iXdata {
	public:
	virtual void Clear()=0;
	virtual bool AppendRecord(const char *data,int id=0)=0;
};


#endif
