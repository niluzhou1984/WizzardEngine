
#ifdef SWIG
%{

#include "We.h"

int main() 
{ 
    AS3_GoAsync();
}
%}

%include "./../../Public/Include/WeCommon.h"
%include "./../../Public/Include/We.h"

#endif



