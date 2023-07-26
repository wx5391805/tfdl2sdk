// Copyright 2022 thinkforce Inc.

#include "onnxruntime_c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \param use_arena zero: false. non-zero: true.
 */
ORT_EXPORT
ORT_API_STATUS(OrtSessionOptionsAppendExecutionProvider_TFACC, _In_ OrtSessionOptions* options)
ORT_ALL_ARGS_NONNULL;

#ifdef __cplusplus
}
#endif
