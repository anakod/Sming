#pragma once

#include "HashContext.h"

namespace Crypto
{
class Sha256Engine
{
public:
	using Hash = HashValue<SHA256_SIZE>;

	Sha256Engine()
	{
		SHA256_Init(&context);
	}

	void update(const void* data, size_t size)
	{
		SHA256_Update(&context, static_cast<const uint8_t*>(data), size);
	}

	void final(Hash& hash)
	{
		SHA256_Final(hash.data, &context);
	}

private:
	SHA256_CTX context;
};

using Sha256 = HashContext<Sha256Engine>;

} // namespace Crypto
