# pico_double

RUNTIME_INIT_FUNC += \
	__aeabi_double_init

WRAPPED_FUNCTIONS += \
	__aeabi_dadd \
	__aeabi_ddiv \
	__aeabi_dmul \
	__aeabi_drsub \
	__aeabi_dsub \
	__aeabi_cdcmpeq \
	__aeabi_cdrcmple \
	__aeabi_cdcmple \
	__aeabi_dcmpeq \
	__aeabi_dcmplt \
	__aeabi_dcmple \
	__aeabi_dcmpge \
	__aeabi_dcmpgt \
	__aeabi_dcmpun \
	__aeabi_i2d \
	__aeabi_l2d \
	__aeabi_ui2d \
	__aeabi_ul2d \
	__aeabi_d2iz \
	__aeabi_d2lz \
	__aeabi_d2uiz \
	__aeabi_d2ulz \
	__aeabi_d2f \
	sqrt \
	cos \
	sin \
	tan \
	atan2 \
	exp \
	log \
	ldexp \
	copysign \
	trunc \
	floor \
	ceil \
	round \
	sincos \
	asin \
	acos \
	atan \
	sinh \
	cosh \
	tanh \
	asinh \
	acosh \
	atanh \
	exp2 \
	log2 \
	exp10 \
	log10 \
	pow \
	powint \
	hypot \
	cbrt \
	fmod \
	drem \
	remainder \
	remquo \
	expm1 \
	log1p \
	fma
