#ifndef KOPE_VULKAN_TEXTURE_STRUCTS_HEADER
#define KOPE_VULKAN_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kope_vulkan_texture {
	uint32_t width;
	uint32_t height;
	VkImage image;
	VkImageView image_view;
} kope_vulkan_texture;

#ifdef __cplusplus
}
#endif

#endif
