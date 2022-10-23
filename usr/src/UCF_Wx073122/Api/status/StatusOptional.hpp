
#ifndef STATUSOPTIONAL
#define STATUSOPTIONAL

namespace DataStatus {

    template<class T>
    class Optional {
        public:
        Optional()=default;
        Optional(T value) 	{ Data=value; DataIsSetFlag=true; }
        void Set (T value) 	{ Data=value; DataIsSetFlag=true; }

        bool IsValid() 		{ return(DataIsSetFlag); }
        T GetValue() 		{ return(Data); }
	operator bool() 	{ return(DataIsSetFlag); }


        private:
        bool DataIsSetFlag {false};
        T Data;
    };
}

#endif
