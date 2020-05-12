#ifndef SLCS_IF_H
#define SLCS_IF_H

class slcs_if
{
	public:
		virtual void release_busy(unsigned id)=0;	
};

#endif
