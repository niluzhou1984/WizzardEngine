%module WizzardEngine

%{
#include "WeCommon.h"
#include "We.h"
%}


%include "carrays.i"
%include "cpointer.i"
%#include "WeCommon.h"
%#include "We.h"
%array_class(float, FloatArray);
