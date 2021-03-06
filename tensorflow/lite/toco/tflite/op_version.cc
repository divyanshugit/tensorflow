/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "tensorflow/lite/toco/tflite/op_version.h"

#include <cstring>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "tensorflow/lite/toco/model.h"
#include "tensorflow/lite/toco/tflite/operator.h"
#include "tensorflow/lite/toco/tooling_util.h"
#include "tensorflow/lite/tools/versioning/runtime_version.h"

namespace toco {
namespace tflite {

// Deprecated and please register new ops/versions in
// tflite/tools/versioning/op_version.cc".
std::string GetMinimumRuntimeVersionForModel(const Model& model) {
  // Use this as the placeholder string if a particular op is not yet included
  // in any Tensorflow's RC/Final release source package. Once that op is
  // included in the release, please update this with the real version string.
  static constexpr char kPendingReleaseOpVersion[] = "UNKNOWN";
  // A map from the version key of an op to its minimum runtime version.
  // For example, {{kAveragePool, 1}, "1.5.0"},  means the 1st version of
  // AveragePool requires a minimum TF Lite runtime version '1.5.0`.
  static const std::map<std::pair<OperatorType, int>, std::string>*
      op_version_map = new std::map<std::pair<OperatorType, int>, std::string>({
          {{OperatorType::kAveragePool, 1}, "1.5.0"},
          {{OperatorType::kAveragePool, 2}, "1.14.0"},
          {{OperatorType::kAveragePool, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kConv, 1}, "1.5.0"},
          {{OperatorType::kConv, 2}, "1.14.0"},
          {{OperatorType::kConv, 3}, "1.14.0"},
          {{OperatorType::kConv, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kDepthwiseConv, 1}, "1.5.0"},
          {{OperatorType::kDepthwiseConv, 2}, "1.12.0"},
          {{OperatorType::kDepthwiseConv, 3}, "1.14.0"},
          {{OperatorType::kDepthwiseConv, 4}, "1.14.0"},
          {{OperatorType::kDepthwiseConv, 5}, kPendingReleaseOpVersion},
          {{OperatorType::kAdd, 1}, "1.5.0"},
          {{OperatorType::kAdd, 2}, "1.14.0"},
          {{OperatorType::kAdd, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kAddN, 1}, "1.14.0"},
          {{OperatorType::kSpaceToBatchND, 1}, "1.6.0"},
          {{OperatorType::kSpaceToBatchND, 2}, "1.14.0"},
          {{OperatorType::kSub, 1}, "1.6.0"},
          {{OperatorType::kSub, 2}, "1.14.0"},
          {{OperatorType::kSub, 3}, "1.15.0"},
          {{OperatorType::kSub, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kSub, 5}, kPendingReleaseOpVersion},
          {{OperatorType::kDiv, 1}, "1.6.0"},
          {{OperatorType::kBatchToSpaceND, 1}, "1.6.0"},
          {{OperatorType::kBatchToSpaceND, 2}, "1.14.0"},
          {{OperatorType::kBatchMatMul, 1}, kPendingReleaseOpVersion},
          {{OperatorType::kCast, 1}, "1.5.0"},
          {{OperatorType::kConcatenation, 1}, "1.5.0"},
          {{OperatorType::kConcatenation, 2}, "1.14.0"},
          {{OperatorType::kConcatenation, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kDepthToSpace, 1}, "2.1.0"},
          {{OperatorType::kFakeQuant, 1}, "1.5.0"},
          {{OperatorType::kFakeQuant, 2}, "1.10.0"},
          {{OperatorType::kFullyConnected, 1}, "1.5.0"},
          {{OperatorType::kFullyConnected, 2}, "1.10.0"},
          {{OperatorType::kFullyConnected, 3}, "1.14.0"},
          {{OperatorType::kFullyConnected, 4}, "1.14.0"},
          {{OperatorType::kFullyConnected, 5}, "2.0.0"},
          {{OperatorType::kFullyConnected, 6}, "2.1.0"},
          {{OperatorType::kFullyConnected, 7}, kPendingReleaseOpVersion},
          {{OperatorType::kGather, 1}, "1.6.0"},
          {{OperatorType::kGather, 2}, "1.14.0"},
          {{OperatorType::kGather, 3}, "1.15.0"},
          {{OperatorType::kGather, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kGatherNd, 1}, "1.14.0"},
          {{OperatorType::kGatherNd, 2}, kPendingReleaseOpVersion},
          {{OperatorType::kSvdf, 1}, "1.5.0"},
          {{OperatorType::kSvdf, 2}, "1.14.0"},
          {{OperatorType::kSvdf, 3}, "2.2.0"},
          {{OperatorType::kL2Normalization, 1}, "1.5.0"},
          {{OperatorType::kL2Normalization, 2}, "1.14.0"},
          {{OperatorType::kL2Pool, 1}, "1.5.0"},
          {{OperatorType::kLocalResponseNormalization, 1}, "1.5.0"},
          {{OperatorType::kMaxPool, 1}, "1.5.0"},
          {{OperatorType::kMaxPool, 2}, "1.14.0"},
          {{OperatorType::kMaxPool, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kMaximum, 1}, "1.14.0"},
          {{OperatorType::kMaximum, 2}, "1.14.0"},
          {{OperatorType::kMaximum, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kMaximum, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kMinimum, 1}, "1.14.0"},
          {{OperatorType::kMinimum, 2}, "1.14.0"},
          {{OperatorType::kMinimum, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kMul, 1}, "1.5.0"},
          {{OperatorType::kMul, 2}, "1.14.0"},
          {{OperatorType::kMul, 3}, "1.15.0"},
          {{OperatorType::kMul, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kPad, 1}, "1.5.0"},
          {{OperatorType::kPad, 2}, "1.14.0"},
          {{OperatorType::kPad, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kTile, 1}, "1.10.1"},
          {{OperatorType::kTile, 2}, kPendingReleaseOpVersion},
          {{OperatorType::kPadV2, 1}, "1.9.0"},
          {{OperatorType::kPadV2, 2}, "1.14.0"},
          {{OperatorType::kPadV2, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kReshape, 1}, "1.5.0"},
          {{OperatorType::kSoftmax, 1}, "1.5.0"},
          {{OperatorType::kSoftmax, 2}, "1.14.0"},
          {{OperatorType::kSoftmax, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kSpaceToDepth, 1}, "1.5.0"},
          {{OperatorType::kSpaceToDepth, 2}, "1.14.0"},
          {{OperatorType::kTranspose, 1}, "1.6.0"},
          {{OperatorType::kTranspose, 2}, "1.14.0"},
          {{OperatorType::kTranspose, 3}, "1.15.0"},
          {{OperatorType::kLstmCell, 1}, "1.7.0"},
          {{OperatorType::kLstmCell, 2}, "1.10.0"},
          {{OperatorType::kLstmCell, 3}, "1.14.0"},
          {{OperatorType::kUnidirectionalSequenceLstm, 1}, "1.13.1"},
          {{OperatorType::kUnidirectionalSequenceLstm, 1}, "1.14.0"},
          {{OperatorType::kBidirectionalSequenceLstm, 1}, "1.14.0"},
          {{OperatorType::kBidirectionalSequenceRnn, 1}, "1.14.0"},
          {{OperatorType::kMean, 1}, "1.6.0"},
          {{OperatorType::kMean, 2}, "1.14.0"},
          {{OperatorType::kMean, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kSum, 1}, "1.10.0"},
          {{OperatorType::kSum, 2}, "1.15.0"},
          {{OperatorType::kReduceMax, 1}, "1.11.0"},
          {{OperatorType::kReduceMax, 2}, "1.14.0"},
          {{OperatorType::kReduceMin, 1}, "1.11.0"},
          {{OperatorType::kReduceMin, 2}, "1.14.0"},
          {{OperatorType::kReduceProd, 1}, "1.11.0"},
          {{OperatorType::kAny, 1}, "1.11.0"},
          {{OperatorType::kRelu6, 1}, "1.5.0"},
          {{OperatorType::kRelu6, 2}, "1.14.0"},
          {{OperatorType::kResizeBilinear, 1}, "1.7.0"},
          {{OperatorType::kResizeBilinear, 2}, "1.14.0"},
          {{OperatorType::kResizeBilinear, 3}, "2.2.0"},
          {{OperatorType::kResizeNearestNeighbor, 1}, "1.13.1"},
          {{OperatorType::kResizeNearestNeighbor, 2}, "1.14.0"},
          {{OperatorType::kResizeNearestNeighbor, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kResizeNearestNeighbor, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kSqueeze, 1}, "1.6.0"},
          {{OperatorType::kSplit, 1}, "1.5.0"},
          {{OperatorType::kSplit, 2}, "1.14.0"},
          {{OperatorType::kSplit, 3}, "1.14.0"},
          {{OperatorType::kSplit, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kSplitV, 2}, kPendingReleaseOpVersion},
          {{OperatorType::kStridedSlice, 1}, "1.6.0"},
          {{OperatorType::kStridedSlice, 2}, "1.14.0"},
          {{OperatorType::kStridedSlice, 3}, "2.1.0"},
          {{OperatorType::kTopK_V2, 1}, "1.7.0"},
          {{OperatorType::kTopK_V2, 2}, "1.14.0"},
          {{OperatorType::kArgMax, 1}, "1.9.0"},
          {{OperatorType::kArgMax, 2}, "1.14.0"},
          {{OperatorType::kArgMin, 1}, "1.9.0"},
          {{OperatorType::kArgMin, 2}, "1.14.0"},
          {{OperatorType::kTransposeConv, 1}, "1.9.0"},
          {{OperatorType::kTransposeConv, 2}, kPendingReleaseOpVersion},
          {{OperatorType::kTransposeConv, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kSparseToDense, 1}, "1.9.0"},
          {{OperatorType::kSparseToDense, 2}, "1.14.0"},
          {{OperatorType::kSparseToDense, 3}, "1.15.0"},
          {{OperatorType::kExpandDims, 1}, "1.10.0"},
          {{OperatorType::kPack, 1}, "1.11.0"},
          {{OperatorType::kPack, 2}, "1.14.0"},
          {{OperatorType::kPack, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kShape, 1}, "1.10.0"},
          {{OperatorType::kSlice, 1}, "1.14.0"},
          {{OperatorType::kSlice, 2}, "1.14.0"},
          {{OperatorType::kSlice, 3}, "1.14.0"},
          {{OperatorType::kTanh, 1}, "1.14.0"},
          {{OperatorType::kTanh, 2}, "1.14.0"},
          {{OperatorType::kTanh, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kOneHot, 1}, "1.11.0"},
          {{OperatorType::kCTCBeamSearchDecoder, 1}, "1.11.0"},
          {{OperatorType::kUnpack, 1}, "1.11.0"},
          {{OperatorType::kUnpack, 2}, "1.14.0"},
          {{OperatorType::kUnpack, 3}, "2.2.0"},
          {{OperatorType::kUnpack, 4}, kPendingReleaseOpVersion},
          {{OperatorType::kLeakyRelu, 1}, "1.13.1"},
          {{OperatorType::kLeakyRelu, 2}, kPendingReleaseOpVersion},
          {{OperatorType::kLogistic, 1}, "1.14.0"},
          {{OperatorType::kLogistic, 2}, "1.14.0"},
          {{OperatorType::kLogistic, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kLogSoftmax, 1}, "1.14.0"},
          {{OperatorType::kLogSoftmax, 2}, "1.14.0"},
          {{OperatorType::kSquaredDifference, 1}, "1.13.1"},
          {{OperatorType::kMirrorPad, 1}, "1.13.1"},
          {{OperatorType::kUnique, 1}, "1.14.0"},
          {{OperatorType::kUnidirectionalSequenceRnn, 1}, "1.14.0"},
          {{OperatorType::kWhere, 1}, "1.14.0"},
          {{OperatorType::kDequantize, 1}, "1.13.1"},
          {{OperatorType::kDequantize, 2}, "1.14.0"},
          {{OperatorType::kDequantize, 3}, "1.15.0"},
          {{OperatorType::kReverseSequence, 1}, "1.14.0"},
          {{OperatorType::kEqual, 1}, "1.14.0"},
          {{OperatorType::kEqual, 2}, "1.14.0"},
          {{OperatorType::kEqual, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kNotEqual, 1}, "1.14.0"},
          {{OperatorType::kNotEqual, 2}, "1.14.0"},
          {{OperatorType::kNotEqual, 3}, kPendingReleaseOpVersion},
          {{OperatorType::kGreater, 1}, "1.14.0"},
          {{OperatorType::kGreater, 2}, "1.14.0"},
          {{OperatorType::kGreaterEqual, 1}, "1.14.0"},
          {{OperatorType::kGreaterEqual, 2}, "1.14.0"},
          {{OperatorType::kLess, 1}, "1.14.0"},
          {{OperatorType::kLess, 2}, "1.14.0"},
          {{OperatorType::kLessEqual, 1}, "1.14.0"},
          {{OperatorType::kLessEqual, 2}, "1.14.0"},
          {{OperatorType::kSegmentSum, 1}, "2.2.0"},
          {{OperatorType::kSelect, 1}, "1.14.0"},
          {{OperatorType::kSelect, 2}, "1.14.0"},
          {{OperatorType::kSelectV2, 1}, "2.2.0"},
          {{OperatorType::kFloorDiv, 1}, "1.14.0"},
          {{OperatorType::kFloorDiv, 2}, "1.14.0"},
          {{OperatorType::kFloor, 1}, "1.9.0"},
          {{OperatorType::kCeil, 1}, "1.14.0"},
          {{OperatorType::kMatrixDiag, 1}, "1.14.0"},
          {{OperatorType::kMatrixSetDiag, 1}, "1.14.0"},
          {{OperatorType::kElu, 1}, "1.14.0"},
          {{OperatorType::kRound, 1}, "1.14.0"},
          {{OperatorType::kRelu, 1}, "1.5.0"},
          {{OperatorType::kRelu, 2}, "2.1.0"},
          {{OperatorType::kRelu1, 1}, "1.5.0"},
          {{OperatorType::kPRelu, 1}, "1.8.0"},
          {{OperatorType::kExp, 1}, "1.7.0"},
          {{OperatorType::kCos, 1}, "1.14.0"},
          {{OperatorType::kNeg, 1}, "1.9.0"},
          {{OperatorType::kPow, 1}, "1.10.0"},
          {{OperatorType::kLogicalOr, 1}, "1.11.0"},
          {{OperatorType::kLogicalAnd, 1}, "1.11.0"},
          {{OperatorType::kLogicalNot, 1}, "1.11.0"},
          {{OperatorType::kFloorMod, 1}, "1.13.0"},
          {{OperatorType::kRange, 1}, "1.13.0"},
          {{OperatorType::kSin, 1}, "1.9.0"},
          {{OperatorType::kLog, 1}, "1.14.0"},
          {{OperatorType::kRsqrt, 1}, "1.10.0"},
          {{OperatorType::kSquare, 1}, "1.12.0"},
          {{OperatorType::kZerosLike, 1}, "1.12.0"},
          {{OperatorType::kAbs, 1}, "1.13.0"},
          {{OperatorType::kHardSwish, 1}, "1.15.0"},
          {{OperatorType::kFill, 1}, "1.13.0"},
          {{OperatorType::kFill, 2}, kPendingReleaseOpVersion},
          {{OperatorType::kReverseV2, 1}, "1.14.0"},
          {{OperatorType::kReverseV2, 2}, "2.2.0"},
          {{OperatorType::kRank, 1}, "1.14.0"},
      });

  const auto& op_types_map =
      tflite::BuildOperatorByTypeMap(false /*enable_select_tf_ops=*/);
  OperatorSignature op_signature;
  op_signature.model = &model;
  std::string model_min_version;
  for (const auto& op : model.operators) {
    if (op_types_map.find(op->type) == op_types_map.end()) continue;
    op_signature.op = op.get();
    const int version = op_types_map.at(op->type)->GetVersion(op_signature);
    std::pair<OperatorType, int> version_key = {op->type, version};
    auto it = op_version_map->find(version_key);
    if (it == op_version_map->end() || it->second == kPendingReleaseOpVersion) {
      // In case we didn't find the current op in the map, or the operator
      // doesn't have a minimum runtime version associated, continue.
      continue;
    }
    if (::tflite::CompareRuntimeVersion(model_min_version, it->second)) {
      // Current min model runtime version should be bumped if we see a higher
      // op version.
      model_min_version = it->second;
    }
  }
  return model_min_version;
}

}  // namespace tflite
}  // namespace toco
