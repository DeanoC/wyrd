#pragma once
#ifndef CORE_BLOB_H_
#define CORE_BLOB_H_ 1

namespace Core
{

struct Blob
{
	uint64_t size;
	void* nativeData;

	static auto Create(uint64_t size_, Blob* out_) -> bool
	{
		if (out_ == nullptr) return false;
		out_->nativeData = malloc(size_);
		if (out_->nativeData == nullptr) return false;
		out_->size = size_;
		return true;
	}

	static auto Free(Blob* in_) -> void
	{
		if (in_ == nullptr) return;
		if (in_->nativeData == nullptr) return;
		free(in_->nativeData);
		in_->size = 0;
		in_->nativeData = nullptr;
	}

};

}

#endif