#include "pipeline_functions.h"
#include "pipeline_structs.h"

#include "shader_structs.h"

#include <kinc/log.h>

#include <kinc/backend/SystemMicrosoft.h>

static D3D12_BLEND convert_blend_factor(kope_d3d12_blend_factor factor) {
	switch (factor) {
	case KOPE_D3D12_BLEND_FACTOR_ZERO:
		return D3D12_BLEND_ZERO;
	case KOPE_D3D12_BLEND_FACTOR_ONE:
		return D3D12_BLEND_ONE;
	case KOPE_D3D12_BLEND_FACTOR_SRC:
		return D3D12_BLEND_SRC_COLOR;
	case KOPE_D3D12_BLEND_FACTOR_ONE_MINUS_SRC:
		return D3D12_BLEND_INV_SRC_COLOR;
	case KOPE_D3D12_BLEND_FACTOR_SRC_ALPHA:
		return D3D12_BLEND_SRC_ALPHA;
	case KOPE_D3D12_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		return D3D12_BLEND_INV_SRC_ALPHA;
	case KOPE_D3D12_BLEND_FACTOR_DST:
		return D3D12_BLEND_DEST_COLOR;
	case KOPE_D3D12_BLEND_FACTOR_ONE_MINUS_DST:
		return D3D12_BLEND_INV_DEST_COLOR;
	case KOPE_D3D12_BLEND_FACTOR_DST_ALPHA:
		return D3D12_BLEND_DEST_ALPHA;
	case KOPE_D3D12_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		return D3D12_BLEND_INV_DEST_ALPHA;
	case KOPE_D3D12_BLEND_FACTOR_SRC_ALPHA_SATURATED:
		return D3D12_BLEND_SRC_ALPHA_SAT;
	case KOPE_D3D12_BLEND_FACTOR_CONSTANT:
		return D3D12_BLEND_BLEND_FACTOR;
	case KOPE_D3D12_BLEND_FACTOR_ONE_MINUS_CONSTANT:
		return D3D12_BLEND_INV_BLEND_FACTOR;
	default:
		assert(false);
		return D3D12_BLEND_ONE;
	}
}

static D3D12_BLEND_OP convert_blend_operation(kope_d3d12_blend_operation op) {
	switch (op) {
	case KOPE_D3D12_BLEND_OPERATION_ADD:
		return D3D12_BLEND_OP_ADD;
	case KOPE_D3D12_BLEND_OPERATION_SUBTRACT:
		return D3D12_BLEND_OP_SUBTRACT;
	case KOPE_D3D12_BLEND_OPERATION_REVERSE_SUBTRACT:
		return D3D12_BLEND_OP_REV_SUBTRACT;
	case KOPE_D3D12_BLEND_OPERATION_MIN:
		return D3D12_BLEND_OP_MIN;
	case KOPE_D3D12_BLEND_OPERATION_MAX:
		return D3D12_BLEND_OP_MAX;
	default:
		assert(false);
		return D3D12_BLEND_OP_ADD;
	}
}

static D3D12_CULL_MODE convert_cull_mode(kope_d3d12_cull_mode mode) {
	switch (mode) {
	case KOPE_D3D12_CULL_MODE_NONE:
		return D3D12_CULL_MODE_NONE;
	case KOPE_D3D12_CULL_MODE_FRONT:
		return D3D12_CULL_MODE_FRONT;
	case KOPE_D3D12_CULL_MODE_BACK:
		return D3D12_CULL_MODE_BACK;
	default:
		assert(false);
		return D3D12_CULL_MODE_NONE;
	}
}

static D3D12_COMPARISON_FUNC convert_compare_function(kope_d3d12_compare_function fun) {
	switch (fun) {
	case KOPE_D3D12_COMPARE_FUNCTION_NEVER:
		return D3D12_COMPARISON_FUNC_NEVER;
	case KOPE_D3D12_COMPARE_FUNCTION_LESS:
		return D3D12_COMPARISON_FUNC_LESS;
	case KOPE_D3D12_COMPARE_FUNCTION_EQUAL:
		return D3D12_COMPARISON_FUNC_EQUAL;
	case KOPE_D3D12_COMPARE_FUNCTION_LESS_EQUAL:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case KOPE_D3D12_COMPARE_FUNCTION_GREATER:
		return D3D12_COMPARISON_FUNC_GREATER;
	case KOPE_D3D12_COMPARE_FUNCTION_NOT_EQUAL:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case KOPE_D3D12_COMPARE_FUNCTION_GREATER_EQUAL:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case KOPE_D3D12_COMPARE_FUNCTION_ALWAYS:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		assert(false);
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

D3D12_STENCIL_OP convert_stencil_operation(kope_d3d12_stencil_operation operation) {
	switch (operation) {
	case KOPE_D3D12_STENCIL_OPERATION_KEEP:
		return D3D12_STENCIL_OP_KEEP;
	case KOPE_D3D12_STENCIL_OPERATION_ZERO:
		return D3D12_STENCIL_OP_ZERO;
	case KOPE_D3D12_STENCIL_OPERATION_REPLACE:
		return D3D12_STENCIL_OP_REPLACE;
	case KOPE_D3D12_STENCIL_OPERATION_INVERT:
		return D3D12_STENCIL_OP_INVERT;
	case KOPE_D3D12_STENCIL_OPERATION_INCREMENT_CLAMP:
		return D3D12_STENCIL_OP_INCR_SAT;
	case KOPE_D3D12_STENCIL_OPERATION_DECREMENT_CLAMP:
		return D3D12_STENCIL_OP_DECR_SAT;
	case KOPE_D3D12_STENCIL_OPERATION_INCREMENT_WRAP:
		return D3D12_STENCIL_OP_INCR;
	case KOPE_D3D12_STENCIL_OPERATION_DECREMENT_WRAP:
		return D3D12_STENCIL_OP_DECR;
	default:
		assert(false);
		return D3D12_STENCIL_OP_KEEP;
	}
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE convert_primitive_topology(kope_d3d12_primitive_topology topolocy) {
	switch (topolocy) {
	case KOPE_D3D12_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case KOPE_D3D12_PRIMITIVE_TOPOLOGY_LINE_LIST:
	case KOPE_D3D12_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case KOPE_D3D12_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
	case KOPE_D3D12_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	default:
		assert(false);
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
}

static void set_blend_state(D3D12_BLEND_DESC *desc, const kope_d3d12_color_target_state *target_state, size_t target) {
	desc->RenderTarget[target].BlendEnable =
	    target_state->blend.color.src_factor != KOPE_D3D12_BLEND_FACTOR_ONE || target_state->blend.color.dst_factor != KOPE_D3D12_BLEND_FACTOR_ZERO ||
	    target_state->blend.alpha.src_factor != KOPE_D3D12_BLEND_FACTOR_ONE || target_state->blend.alpha.dst_factor != KOPE_D3D12_BLEND_FACTOR_ZERO;
	desc->RenderTarget[target].SrcBlend = convert_blend_factor(target_state->blend.color.src_factor);
	desc->RenderTarget[target].DestBlend = convert_blend_factor(target_state->blend.color.dst_factor);
	desc->RenderTarget[target].BlendOp = convert_blend_operation(target_state->blend.color.operation);
	desc->RenderTarget[target].SrcBlendAlpha = convert_blend_factor(target_state->blend.alpha.src_factor);
	desc->RenderTarget[target].DestBlendAlpha = convert_blend_factor(target_state->blend.alpha.dst_factor);
	desc->RenderTarget[target].BlendOpAlpha = convert_blend_operation(target_state->blend.alpha.operation);
	desc->RenderTarget[target].RenderTargetWriteMask = target_state->write_mask;
}

void kope_d3d12_pipeline_init(kope_d3d12_device *device, kope_d3d12_pipeline *pipe, const kope_d3d12_pipeline_parameters *parameters) {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {0};

	desc.VS.BytecodeLength = parameters->vertex.shader->size;
	desc.VS.pShaderBytecode = parameters->vertex.shader->data;

	assert(parameters->vertex.buffers_count <= KOPE_D3D12_MAX_VERTEX_ATTRIBUTES);
	D3D12_INPUT_ELEMENT_DESC input_elements[KOPE_D3D12_MAX_VERTEX_ATTRIBUTES] = {0};

	size_t input_element_index = 0;
	for (size_t buffer_index = 0; buffer_index < parameters->vertex.buffers_count; ++buffer_index) {
		for (size_t attribute_index = 0; attribute_index < parameters->vertex.buffers[buffer_index].attributes_count; ++attribute_index) {
			input_elements[input_element_index].SemanticName = "TEXCOORD";
			input_elements[input_element_index].SemanticIndex = (UINT)attribute_index;
			input_elements[input_element_index].InputSlot = (UINT)buffer_index;
			input_elements[input_element_index].AlignedByteOffset = (attribute_index == 0) ? 0 : D3D12_APPEND_ALIGNED_ELEMENT;
			input_elements[input_element_index].InputSlotClass = parameters->vertex.buffers[buffer_index].step_mode == KOPE_D3D12_VERTEX_STEP_MODE_INSTANCE
			                                                         ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA
			                                                         : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			input_elements[input_element_index].InstanceDataStepRate =
			    parameters->vertex.buffers[buffer_index].step_mode == KOPE_D3D12_VERTEX_STEP_MODE_INSTANCE ? 1 : 0;

			switch (parameters->vertex.buffers[buffer_index].attributes[attribute_index].format) {
			case KOPE_D3D12_VERTEX_FORMAT_UINT8X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT8X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT8X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT8X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8B8A8_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UNORM8X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8_UNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UNORM8X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SNORM8X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8_SNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SNORM8X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R8G8B8A8_SNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT16X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT16X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16B16A16_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT16X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT16X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16B16A16_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UNORM16X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16_UNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UNORM16X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16B16A16_UNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SNORM16X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16_SNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SNORM16X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16B16A16_SNORM;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_FLOAT16X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16_FLOAT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_FLOAT16X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_FLOAT32:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_FLOAT32X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_FLOAT32X3:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_FLOAT32X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT32:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT32X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT32X3:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32B32_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UINT32X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SIN32:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT32X2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT32X3:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32B32_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_SINT32X4:
				input_elements[input_element_index].Format = DXGI_FORMAT_R32G32B32A32_SINT;
				break;
			case KOPE_D3D12_VERTEX_FORMAT_UNORM10_10_10_2:
				input_elements[input_element_index].Format = DXGI_FORMAT_R10G10B10A2_UNORM;
				break;
			default:
				assert(false);
				break;
			}
			input_element_index += 1;
		}
	}

	desc.InputLayout.NumElements = (UINT)input_element_index;
	desc.InputLayout.pInputElementDescs = input_elements;

	desc.PrimitiveTopologyType = convert_primitive_topology(parameters->primitive.topology);

	desc.RasterizerState.FrontCounterClockwise = parameters->primitive.frontFace == KOPE_D3D12_FRONT_FACE_CCW ? TRUE : FALSE;
	desc.RasterizerState.CullMode = convert_cull_mode(parameters->primitive.cull_mode);

	desc.RasterizerState.DepthClipEnable = parameters->primitive.unclipped_depth ? FALSE : TRUE;

	desc.DSVFormat = convert_texture_format(parameters->depth_stencil.format);

	desc.DepthStencilState.DepthEnable = parameters->depth_stencil.depth_compare != KOPE_D3D12_COMPARE_FUNCTION_ALWAYS;
	desc.DepthStencilState.DepthWriteMask = parameters->depth_stencil.depth_write_enabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthStencilState.DepthFunc = convert_compare_function(parameters->depth_stencil.depth_compare);

	desc.DepthStencilState.StencilEnable = parameters->depth_stencil.stencil_front.compare != KOPE_D3D12_COMPARE_FUNCTION_ALWAYS ||
	                                       parameters->depth_stencil.stencil_front.pass_op != KOPE_D3D12_STENCIL_OPERATION_KEEP ||
	                                       parameters->depth_stencil.stencil_front.fail_op != KOPE_D3D12_STENCIL_OPERATION_KEEP ||
	                                       parameters->depth_stencil.stencil_front.depth_fail_op != KOPE_D3D12_STENCIL_OPERATION_KEEP ||
	                                       parameters->depth_stencil.stencil_back.compare != KOPE_D3D12_COMPARE_FUNCTION_ALWAYS ||
	                                       parameters->depth_stencil.stencil_back.pass_op != KOPE_D3D12_STENCIL_OPERATION_KEEP ||
	                                       parameters->depth_stencil.stencil_back.fail_op != KOPE_D3D12_STENCIL_OPERATION_KEEP ||
	                                       parameters->depth_stencil.stencil_back.depth_fail_op != KOPE_D3D12_STENCIL_OPERATION_KEEP;
	desc.DepthStencilState.StencilReadMask = parameters->depth_stencil.stencil_read_mask;
	desc.DepthStencilState.StencilWriteMask = parameters->depth_stencil.stencil_write_mask;
	desc.DepthStencilState.FrontFace.StencilFunc = convert_compare_function(parameters->depth_stencil.stencil_front.compare);
	desc.DepthStencilState.FrontFace.StencilPassOp = convert_stencil_operation(parameters->depth_stencil.stencil_front.pass_op);
	desc.DepthStencilState.FrontFace.StencilFailOp = convert_stencil_operation(parameters->depth_stencil.stencil_front.fail_op);
	desc.DepthStencilState.FrontFace.StencilDepthFailOp = convert_stencil_operation(parameters->depth_stencil.stencil_front.depth_fail_op);
	desc.DepthStencilState.BackFace.StencilFunc = convert_compare_function(parameters->depth_stencil.stencil_back.compare);
	desc.DepthStencilState.BackFace.StencilPassOp = convert_stencil_operation(parameters->depth_stencil.stencil_back.pass_op);
	desc.DepthStencilState.BackFace.StencilFailOp = convert_stencil_operation(parameters->depth_stencil.stencil_back.fail_op);
	desc.DepthStencilState.BackFace.StencilDepthFailOp = convert_stencil_operation(parameters->depth_stencil.stencil_back.depth_fail_op);

	desc.RasterizerState.DepthBias = parameters->depth_stencil.depth_bias;
	desc.RasterizerState.SlopeScaledDepthBias = parameters->depth_stencil.depth_bias_slope_scale;
	desc.RasterizerState.DepthBiasClamp = parameters->depth_stencil.depth_bias_clamp;

	desc.RasterizerState.MultisampleEnable = parameters->multisample.count > 1 ? TRUE : FALSE;
	desc.SampleDesc.Count = parameters->multisample.count;
	desc.SampleDesc.Quality = 0;
	desc.SampleMask = 0xFFFFFFFF;
	desc.BlendState.AlphaToCoverageEnable = parameters->multisample.alpha_to_coverage_enabled ? TRUE : FALSE;

	desc.PS.BytecodeLength = parameters->fragment.shader->size;
	desc.PS.pShaderBytecode = parameters->fragment.shader->data;

	desc.NumRenderTargets = (UINT)parameters->fragment.targets_count;
	assert(parameters->fragment.targets_count <= KOPE_D3D12_MAX_COLOR_TARGETS);
	for (size_t target_index = 0; target_index < parameters->fragment.targets_count; ++target_index) {
		desc.RTVFormats[target_index] = convert_texture_format(parameters->fragment.targets[target_index].format);
	}

	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	desc.RasterizerState.AntialiasedLineEnable = FALSE;
	desc.RasterizerState.ForcedSampleCount = 0;
	desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	desc.BlendState.IndependentBlendEnable = FALSE;

	bool independent_blend = false;
	for (int i = 1; i < KOPE_D3D12_MAX_COLOR_TARGETS; ++i) {
		if (parameters->fragment.targets[0].write_mask != parameters->fragment.targets[i].write_mask) {
			independent_blend = true;
			break;
		}
	}

	set_blend_state(&desc.BlendState, &parameters->fragment.targets[0], 0);
	if (independent_blend) {
		desc.BlendState.IndependentBlendEnable = true;
		for (int i = 1; i < KOPE_D3D12_MAX_COLOR_TARGETS; ++i) {
			set_blend_state(&desc.BlendState, &parameters->fragment.targets[i], i);
		}
	}

	desc.pRootSignature = NULL;

	HRESULT result = device->device->CreateGraphicsPipelineState(&desc, IID_GRAPHICS_PPV_ARGS(&pipe->pipe));
	if (result != S_OK) {
		kinc_log(KINC_LOG_LEVEL_WARNING, "Could not create pipeline.");
	}
}

void kope_d3d12_pipeline_destroy(kope_d3d12_pipeline *pipe) {
	if (pipe->pipe != NULL) {
		pipe->pipe->Release();
		pipe->pipe = NULL;
	}
}
