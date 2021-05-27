#pragma once
class GNSSANL
{
public:
 	virtual int StreamAnaylse(unsigned long len, char* RxBuffer) = 0;
};

