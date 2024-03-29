/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        vk.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vk.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef VK_NO_PROTOTYPES
#   define LX_VK_API_DEFINE(name) PFN_##name name;
#   define LX_VK_API_LOAD(name) do {\
    name = (PFN_##name)lx_dlsym(s_library, #name);\
    if (!name) {\
        lx_trace_w("%s not found!", #name); \
    }\
} while (0)
#else
#   define LX_VK_API_DEFINE(name)
#   define LX_VK_API_LOAD(name)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// instance extensions
static lx_char_t**          g_instance_extensions = lx_null;
static lx_uint32_t          g_instance_extensions_count = 0;
static lx_uint32_t          g_instance_extensions_maxn = 0;

// device extensions
static lx_char_t**          g_device_extensions = lx_null;
static lx_uint32_t          g_device_extensions_count = 0;
static lx_uint32_t          g_device_extensions_maxn = 0;

// validation layers
static lx_char_t**          g_validation_layers = lx_null;
static lx_uint32_t          g_validation_layers_count = 0;
static lx_uint32_t          g_validation_layers_maxn = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * apis
 */
LX_VK_API_DEFINE(vkCreateInstance);
LX_VK_API_DEFINE(vkDestroyInstance);
LX_VK_API_DEFINE(vkEnumeratePhysicalDevices);
LX_VK_API_DEFINE(vkGetPhysicalDeviceFeatures);
LX_VK_API_DEFINE(vkGetPhysicalDeviceFormatProperties);
LX_VK_API_DEFINE(vkGetPhysicalDeviceImageFormatProperties);
LX_VK_API_DEFINE(vkGetPhysicalDeviceProperties);
LX_VK_API_DEFINE(vkGetPhysicalDeviceQueueFamilyProperties);
LX_VK_API_DEFINE(vkGetPhysicalDeviceMemoryProperties);
LX_VK_API_DEFINE(vkGetInstanceProcAddr);
LX_VK_API_DEFINE(vkGetDeviceProcAddr);
LX_VK_API_DEFINE(vkCreateDevice);
LX_VK_API_DEFINE(vkDestroyDevice);
LX_VK_API_DEFINE(vkEnumerateInstanceExtensionProperties);
LX_VK_API_DEFINE(vkEnumerateDeviceExtensionProperties);
LX_VK_API_DEFINE(vkEnumerateInstanceLayerProperties);
LX_VK_API_DEFINE(vkEnumerateDeviceLayerProperties);
LX_VK_API_DEFINE(vkGetDeviceQueue);
LX_VK_API_DEFINE(vkQueueSubmit);
LX_VK_API_DEFINE(vkQueueWaitIdle);
LX_VK_API_DEFINE(vkDeviceWaitIdle);
LX_VK_API_DEFINE(vkAllocateMemory);
LX_VK_API_DEFINE(vkFreeMemory);
LX_VK_API_DEFINE(vkMapMemory);
LX_VK_API_DEFINE(vkUnmapMemory);
LX_VK_API_DEFINE(vkFlushMappedMemoryRanges);
LX_VK_API_DEFINE(vkInvalidateMappedMemoryRanges);
LX_VK_API_DEFINE(vkGetDeviceMemoryCommitment);
LX_VK_API_DEFINE(vkBindBufferMemory);
LX_VK_API_DEFINE(vkBindImageMemory);
LX_VK_API_DEFINE(vkGetBufferMemoryRequirements);
LX_VK_API_DEFINE(vkGetImageMemoryRequirements);
LX_VK_API_DEFINE(vkGetImageSparseMemoryRequirements);
LX_VK_API_DEFINE(vkGetPhysicalDeviceSparseImageFormatProperties);
LX_VK_API_DEFINE(vkQueueBindSparse);
LX_VK_API_DEFINE(vkCreateFence);
LX_VK_API_DEFINE(vkDestroyFence);
LX_VK_API_DEFINE(vkResetFences);
LX_VK_API_DEFINE(vkGetFenceStatus);
LX_VK_API_DEFINE(vkWaitForFences);
LX_VK_API_DEFINE(vkCreateSemaphore);
LX_VK_API_DEFINE(vkDestroySemaphore);
LX_VK_API_DEFINE(vkCreateEvent);
LX_VK_API_DEFINE(vkDestroyEvent);
LX_VK_API_DEFINE(vkGetEventStatus);
LX_VK_API_DEFINE(vkSetEvent);
LX_VK_API_DEFINE(vkResetEvent);
LX_VK_API_DEFINE(vkCreateQueryPool);
LX_VK_API_DEFINE(vkDestroyQueryPool);
LX_VK_API_DEFINE(vkGetQueryPoolResults);
LX_VK_API_DEFINE(vkCreateBuffer);
LX_VK_API_DEFINE(vkDestroyBuffer);
LX_VK_API_DEFINE(vkCreateBufferView);
LX_VK_API_DEFINE(vkDestroyBufferView);
LX_VK_API_DEFINE(vkCreateImage);
LX_VK_API_DEFINE(vkDestroyImage);
LX_VK_API_DEFINE(vkGetImageSubresourceLayout);
LX_VK_API_DEFINE(vkCreateImageView);
LX_VK_API_DEFINE(vkDestroyImageView);
LX_VK_API_DEFINE(vkCreateShaderModule);
LX_VK_API_DEFINE(vkDestroyShaderModule);
LX_VK_API_DEFINE(vkCreatePipelineCache);
LX_VK_API_DEFINE(vkDestroyPipelineCache);
LX_VK_API_DEFINE(vkGetPipelineCacheData);
LX_VK_API_DEFINE(vkMergePipelineCaches);
LX_VK_API_DEFINE(vkCreateGraphicsPipelines);
LX_VK_API_DEFINE(vkCreateComputePipelines);
LX_VK_API_DEFINE(vkDestroyPipeline);
LX_VK_API_DEFINE(vkCreatePipelineLayout);
LX_VK_API_DEFINE(vkDestroyPipelineLayout);
LX_VK_API_DEFINE(vkCreateSampler);
LX_VK_API_DEFINE(vkDestroySampler);
LX_VK_API_DEFINE(vkCreateDescriptorSetLayout);
LX_VK_API_DEFINE(vkDestroyDescriptorSetLayout);
LX_VK_API_DEFINE(vkCreateDescriptorPool);
LX_VK_API_DEFINE(vkDestroyDescriptorPool);
LX_VK_API_DEFINE(vkResetDescriptorPool);
LX_VK_API_DEFINE(vkAllocateDescriptorSets);
LX_VK_API_DEFINE(vkFreeDescriptorSets);
LX_VK_API_DEFINE(vkUpdateDescriptorSets);
LX_VK_API_DEFINE(vkCreateFramebuffer);
LX_VK_API_DEFINE(vkDestroyFramebuffer);
LX_VK_API_DEFINE(vkCreateRenderPass);
LX_VK_API_DEFINE(vkDestroyRenderPass);
LX_VK_API_DEFINE(vkGetRenderAreaGranularity);
LX_VK_API_DEFINE(vkCreateCommandPool);
LX_VK_API_DEFINE(vkDestroyCommandPool);
LX_VK_API_DEFINE(vkResetCommandPool);
LX_VK_API_DEFINE(vkAllocateCommandBuffers);
LX_VK_API_DEFINE(vkFreeCommandBuffers);
LX_VK_API_DEFINE(vkBeginCommandBuffer);
LX_VK_API_DEFINE(vkEndCommandBuffer);
LX_VK_API_DEFINE(vkResetCommandBuffer);
LX_VK_API_DEFINE(vkCmdBindPipeline);
LX_VK_API_DEFINE(vkCmdSetViewport);
LX_VK_API_DEFINE(vkCmdSetScissor);
LX_VK_API_DEFINE(vkCmdSetLineWidth);
LX_VK_API_DEFINE(vkCmdSetDepthBias);
LX_VK_API_DEFINE(vkCmdSetBlendConstants);
LX_VK_API_DEFINE(vkCmdSetDepthBounds);
LX_VK_API_DEFINE(vkCmdSetStencilCompareMask);
LX_VK_API_DEFINE(vkCmdSetStencilWriteMask);
LX_VK_API_DEFINE(vkCmdSetStencilReference);
LX_VK_API_DEFINE(vkCmdBindDescriptorSets);
LX_VK_API_DEFINE(vkCmdBindIndexBuffer);
LX_VK_API_DEFINE(vkCmdBindVertexBuffers);
LX_VK_API_DEFINE(vkCmdDraw);
LX_VK_API_DEFINE(vkCmdDrawIndexed);
LX_VK_API_DEFINE(vkCmdDrawIndirect);
LX_VK_API_DEFINE(vkCmdDrawIndexedIndirect);
LX_VK_API_DEFINE(vkCmdDispatch);
LX_VK_API_DEFINE(vkCmdDispatchIndirect);
LX_VK_API_DEFINE(vkCmdCopyBuffer);
LX_VK_API_DEFINE(vkCmdCopyImage);
LX_VK_API_DEFINE(vkCmdBlitImage);
LX_VK_API_DEFINE(vkCmdCopyBufferToImage);
LX_VK_API_DEFINE(vkCmdCopyImageToBuffer);
LX_VK_API_DEFINE(vkCmdUpdateBuffer);
LX_VK_API_DEFINE(vkCmdFillBuffer);
LX_VK_API_DEFINE(vkCmdClearColorImage);
LX_VK_API_DEFINE(vkCmdClearDepthStencilImage);
LX_VK_API_DEFINE(vkCmdClearAttachments);
LX_VK_API_DEFINE(vkCmdResolveImage);
LX_VK_API_DEFINE(vkCmdSetEvent);
LX_VK_API_DEFINE(vkCmdResetEvent);
LX_VK_API_DEFINE(vkCmdWaitEvents);
LX_VK_API_DEFINE(vkCmdPipelineBarrier);
LX_VK_API_DEFINE(vkCmdBeginQuery);
LX_VK_API_DEFINE(vkCmdEndQuery);
LX_VK_API_DEFINE(vkCmdResetQueryPool);
LX_VK_API_DEFINE(vkCmdWriteTimestamp);
LX_VK_API_DEFINE(vkCmdCopyQueryPoolResults);
LX_VK_API_DEFINE(vkCmdPushConstants);
LX_VK_API_DEFINE(vkCmdBeginRenderPass);
LX_VK_API_DEFINE(vkCmdNextSubpass);
LX_VK_API_DEFINE(vkCmdEndRenderPass);
LX_VK_API_DEFINE(vkCmdExecuteCommands);

// VK_KHR_surface
LX_VK_API_DEFINE(vkDestroySurfaceKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceSurfaceSupportKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceSurfaceFormatsKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceSurfacePresentModesKHR);

// VK_KHR_swapchain
LX_VK_API_DEFINE(vkCreateSwapchainKHR);
LX_VK_API_DEFINE(vkDestroySwapchainKHR);
LX_VK_API_DEFINE(vkGetSwapchainImagesKHR);
LX_VK_API_DEFINE(vkAcquireNextImageKHR);
LX_VK_API_DEFINE(vkQueuePresentKHR);

// VK_KHR_display
LX_VK_API_DEFINE(vkGetPhysicalDeviceDisplayPropertiesKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
LX_VK_API_DEFINE(vkGetDisplayPlaneSupportedDisplaysKHR);
LX_VK_API_DEFINE(vkGetDisplayModePropertiesKHR);
LX_VK_API_DEFINE(vkCreateDisplayModeKHR);
LX_VK_API_DEFINE(vkGetDisplayPlaneCapabilitiesKHR);
LX_VK_API_DEFINE(vkCreateDisplayPlaneSurfaceKHR);

// VK_KHR_display_swapchain
LX_VK_API_DEFINE(vkCreateSharedSwapchainsKHR);

#ifdef VK_USE_PLATFORM_XLIB_KHR
// VK_KHR_xlib_surface
LX_VK_API_DEFINE(vkCreateXlibSurfaceKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
// VK_KHR_xcb_surface
LX_VK_API_DEFINE(vkCreateXcbSurfaceKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
// VK_KHR_wayland_surface
LX_VK_API_DEFINE(vkCreateWaylandSurfaceKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
// VK_KHR_mir_surface
LX_VK_API_DEFINE(vkCreateMirSurfaceKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceMirPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
// VK_KHR_android_surface
LX_VK_API_DEFINE(vkCreateAndroidSurfaceKHR);
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
// VK_KHR_win32_surface
LX_VK_API_DEFINE(vkCreateWin32SurfaceKHR);
LX_VK_API_DEFINE(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

/* we need check the below supports
 *
 * - graphics family queue
 * - VK_KHR_swapchain (device extensions)
 */
static lx_inline lx_bool_t lx_vk_device_is_suitable(VkPhysicalDevice device) {
    static lx_char_t const* device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    return lx_vk_physical_device_find_family_queue(device, VK_QUEUE_GRAPHICS_BIT) >= 0 &&
        lx_vk_device_extensions_check(device, device_extensions, lx_arrayn(device_extensions));
}

#ifdef LX_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL lx_vk_debug_utils_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (pCallbackData && pCallbackData->pMessage) {
        lx_trace_i("[debug]: %s", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL lx_vk_debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
    size_t location, int32_t messageCode, lx_char_t const* pLayerPrefix, lx_char_t const* pMessage, lx_pointer_t pUserData) {
    if (pMessage) {
        lx_trace_i("[debug]: %s", pMessage);
    }
    return VK_TRUE;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_vk_context_init() {
#ifdef VK_NO_PROTOTYPES
    // load symbols from libvulkan.so
    static lx_dlimage_ref_t s_library = lx_null;
    if (!s_library) {
        s_library = lx_dlopen("libvulkan.so", LX_RTLD_LAZY);
        lx_assert_and_check_return_val(s_library, lx_false);

        LX_VK_API_LOAD(vkCreateInstance);
        LX_VK_API_LOAD(vkDestroyInstance);
        LX_VK_API_LOAD(vkEnumeratePhysicalDevices);
        LX_VK_API_LOAD(vkGetPhysicalDeviceFeatures);
        LX_VK_API_LOAD(vkGetPhysicalDeviceFormatProperties);
        LX_VK_API_LOAD(vkGetPhysicalDeviceImageFormatProperties);
        LX_VK_API_LOAD(vkGetPhysicalDeviceProperties);
        LX_VK_API_LOAD(vkGetPhysicalDeviceQueueFamilyProperties);
        LX_VK_API_LOAD(vkGetPhysicalDeviceMemoryProperties);
        LX_VK_API_LOAD(vkGetInstanceProcAddr);
        LX_VK_API_LOAD(vkGetDeviceProcAddr);
        LX_VK_API_LOAD(vkCreateDevice);
        LX_VK_API_LOAD(vkDestroyDevice);
        LX_VK_API_LOAD(vkEnumerateInstanceExtensionProperties);
        LX_VK_API_LOAD(vkEnumerateDeviceExtensionProperties);
        LX_VK_API_LOAD(vkEnumerateInstanceLayerProperties);
        LX_VK_API_LOAD(vkEnumerateDeviceLayerProperties);
        LX_VK_API_LOAD(vkGetDeviceQueue);
        LX_VK_API_LOAD(vkQueueSubmit);
        LX_VK_API_LOAD(vkQueueWaitIdle);
        LX_VK_API_LOAD(vkDeviceWaitIdle);
        LX_VK_API_LOAD(vkAllocateMemory);
        LX_VK_API_LOAD(vkFreeMemory);
        LX_VK_API_LOAD(vkMapMemory);
        LX_VK_API_LOAD(vkUnmapMemory);
        LX_VK_API_LOAD(vkFlushMappedMemoryRanges);
        LX_VK_API_LOAD(vkInvalidateMappedMemoryRanges);
        LX_VK_API_LOAD(vkGetDeviceMemoryCommitment);
        LX_VK_API_LOAD(vkBindBufferMemory);
        LX_VK_API_LOAD(vkBindImageMemory);
        LX_VK_API_LOAD(vkGetBufferMemoryRequirements);
        LX_VK_API_LOAD(vkGetImageMemoryRequirements);
        LX_VK_API_LOAD(vkGetImageSparseMemoryRequirements);
        LX_VK_API_LOAD(vkGetPhysicalDeviceSparseImageFormatProperties);
        LX_VK_API_LOAD(vkQueueBindSparse);
        LX_VK_API_LOAD(vkCreateFence);
        LX_VK_API_LOAD(vkDestroyFence);
        LX_VK_API_LOAD(vkResetFences);
        LX_VK_API_LOAD(vkGetFenceStatus);
        LX_VK_API_LOAD(vkWaitForFences);
        LX_VK_API_LOAD(vkCreateSemaphore);
        LX_VK_API_LOAD(vkDestroySemaphore);
        LX_VK_API_LOAD(vkCreateEvent);
        LX_VK_API_LOAD(vkDestroyEvent);
        LX_VK_API_LOAD(vkGetEventStatus);
        LX_VK_API_LOAD(vkSetEvent);
        LX_VK_API_LOAD(vkResetEvent);
        LX_VK_API_LOAD(vkCreateQueryPool);
        LX_VK_API_LOAD(vkDestroyQueryPool);
        LX_VK_API_LOAD(vkGetQueryPoolResults);
        LX_VK_API_LOAD(vkCreateBuffer);
        LX_VK_API_LOAD(vkDestroyBuffer);
        LX_VK_API_LOAD(vkCreateBufferView);
        LX_VK_API_LOAD(vkDestroyBufferView);
        LX_VK_API_LOAD(vkCreateImage);
        LX_VK_API_LOAD(vkDestroyImage);
        LX_VK_API_LOAD(vkGetImageSubresourceLayout);
        LX_VK_API_LOAD(vkCreateImageView);
        LX_VK_API_LOAD(vkDestroyImageView);
        LX_VK_API_LOAD(vkCreateShaderModule);
        LX_VK_API_LOAD(vkDestroyShaderModule);
        LX_VK_API_LOAD(vkCreatePipelineCache);
        LX_VK_API_LOAD(vkDestroyPipelineCache);
        LX_VK_API_LOAD(vkGetPipelineCacheData);
        LX_VK_API_LOAD(vkMergePipelineCaches);
        LX_VK_API_LOAD(vkCreateGraphicsPipelines);
        LX_VK_API_LOAD(vkCreateComputePipelines);
        LX_VK_API_LOAD(vkDestroyPipeline);
        LX_VK_API_LOAD(vkCreatePipelineLayout);
        LX_VK_API_LOAD(vkDestroyPipelineLayout);
        LX_VK_API_LOAD(vkCreateSampler);
        LX_VK_API_LOAD(vkDestroySampler);
        LX_VK_API_LOAD(vkCreateDescriptorSetLayout);
        LX_VK_API_LOAD(vkDestroyDescriptorSetLayout);
        LX_VK_API_LOAD(vkCreateDescriptorPool);
        LX_VK_API_LOAD(vkDestroyDescriptorPool);
        LX_VK_API_LOAD(vkResetDescriptorPool);
        LX_VK_API_LOAD(vkAllocateDescriptorSets);
        LX_VK_API_LOAD(vkFreeDescriptorSets);
        LX_VK_API_LOAD(vkUpdateDescriptorSets);
        LX_VK_API_LOAD(vkCreateFramebuffer);
        LX_VK_API_LOAD(vkDestroyFramebuffer);
        LX_VK_API_LOAD(vkCreateRenderPass);
        LX_VK_API_LOAD(vkDestroyRenderPass);
        LX_VK_API_LOAD(vkGetRenderAreaGranularity);
        LX_VK_API_LOAD(vkCreateCommandPool);
        LX_VK_API_LOAD(vkDestroyCommandPool);
        LX_VK_API_LOAD(vkResetCommandPool);
        LX_VK_API_LOAD(vkAllocateCommandBuffers);
        LX_VK_API_LOAD(vkFreeCommandBuffers);
        LX_VK_API_LOAD(vkBeginCommandBuffer);
        LX_VK_API_LOAD(vkEndCommandBuffer);
        LX_VK_API_LOAD(vkResetCommandBuffer);
        LX_VK_API_LOAD(vkCmdBindPipeline);
        LX_VK_API_LOAD(vkCmdSetViewport);
        LX_VK_API_LOAD(vkCmdSetScissor);
        LX_VK_API_LOAD(vkCmdSetLineWidth);
        LX_VK_API_LOAD(vkCmdSetDepthBias);
        LX_VK_API_LOAD(vkCmdSetBlendConstants);
        LX_VK_API_LOAD(vkCmdSetDepthBounds);
        LX_VK_API_LOAD(vkCmdSetStencilCompareMask);
        LX_VK_API_LOAD(vkCmdSetStencilWriteMask);
        LX_VK_API_LOAD(vkCmdSetStencilReference);
        LX_VK_API_LOAD(vkCmdBindDescriptorSets);
        LX_VK_API_LOAD(vkCmdBindIndexBuffer);
        LX_VK_API_LOAD(vkCmdBindVertexBuffers);
        LX_VK_API_LOAD(vkCmdDraw);
        LX_VK_API_LOAD(vkCmdDrawIndexed);
        LX_VK_API_LOAD(vkCmdDrawIndirect);
        LX_VK_API_LOAD(vkCmdDrawIndexedIndirect);
        LX_VK_API_LOAD(vkCmdDispatch);
        LX_VK_API_LOAD(vkCmdDispatchIndirect);
        LX_VK_API_LOAD(vkCmdCopyBuffer);
        LX_VK_API_LOAD(vkCmdCopyImage);
        LX_VK_API_LOAD(vkCmdBlitImage);
        LX_VK_API_LOAD(vkCmdCopyBufferToImage);
        LX_VK_API_LOAD(vkCmdCopyImageToBuffer);
        LX_VK_API_LOAD(vkCmdUpdateBuffer);
        LX_VK_API_LOAD(vkCmdFillBuffer);
        LX_VK_API_LOAD(vkCmdClearColorImage);
        LX_VK_API_LOAD(vkCmdClearDepthStencilImage);
        LX_VK_API_LOAD(vkCmdClearAttachments);
        LX_VK_API_LOAD(vkCmdResolveImage);
        LX_VK_API_LOAD(vkCmdSetEvent);
        LX_VK_API_LOAD(vkCmdResetEvent);
        LX_VK_API_LOAD(vkCmdWaitEvents);
        LX_VK_API_LOAD(vkCmdPipelineBarrier);
        LX_VK_API_LOAD(vkCmdBeginQuery);
        LX_VK_API_LOAD(vkCmdEndQuery);
        LX_VK_API_LOAD(vkCmdResetQueryPool);
        LX_VK_API_LOAD(vkCmdWriteTimestamp);
        LX_VK_API_LOAD(vkCmdCopyQueryPoolResults);
        LX_VK_API_LOAD(vkCmdPushConstants);
        LX_VK_API_LOAD(vkCmdBeginRenderPass);
        LX_VK_API_LOAD(vkCmdNextSubpass);
        LX_VK_API_LOAD(vkCmdEndRenderPass);
        LX_VK_API_LOAD(vkCmdExecuteCommands);

        // VK_KHR_surface
        LX_VK_API_LOAD(vkDestroySurfaceKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceSurfaceSupportKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceSurfaceFormatsKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceSurfacePresentModesKHR);

        // VK_KHR_swapchain
        LX_VK_API_LOAD(vkCreateSwapchainKHR);
        LX_VK_API_LOAD(vkDestroySwapchainKHR);
        LX_VK_API_LOAD(vkGetSwapchainImagesKHR);
        LX_VK_API_LOAD(vkAcquireNextImageKHR);
        LX_VK_API_LOAD(vkQueuePresentKHR);

        // VK_KHR_display
        LX_VK_API_LOAD(vkGetPhysicalDeviceDisplayPropertiesKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
        LX_VK_API_LOAD(vkGetDisplayPlaneSupportedDisplaysKHR);
        LX_VK_API_LOAD(vkGetDisplayModePropertiesKHR);
        LX_VK_API_LOAD(vkCreateDisplayModeKHR);
        LX_VK_API_LOAD(vkGetDisplayPlaneCapabilitiesKHR);
        LX_VK_API_LOAD(vkCreateDisplayPlaneSurfaceKHR);

        // VK_KHR_display_swapchain
        LX_VK_API_LOAD(vkCreateSharedSwapchainsKHR);

#ifdef VK_USE_PLATFORM_XLIB_KHR
        // VK_KHR_xlib_surface
        LX_VK_API_LOAD(vkCreateXlibSurfaceKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
        // VK_KHR_xcb_surface
        LX_VK_API_LOAD(vkCreateXcbSurfaceKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        // VK_KHR_wayland_surface
        LX_VK_API_LOAD(vkCreateWaylandSurfaceKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
        // VK_KHR_mir_surface
        LX_VK_API_LOAD(vkCreateMirSurfaceKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceMirPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
        // VK_KHR_android_surface
        LX_VK_API_LOAD(vkCreateAndroidSurfaceKHR);
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
        // VK_KHR_win32_surface
        LX_VK_API_LOAD(vkCreateWin32SurfaceKHR);
        LX_VK_API_LOAD(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif
    }
#endif
    return lx_true;
}

lx_void_t lx_vk_context_exit() {
    if (g_instance_extensions) {
        lx_free(g_instance_extensions);
        g_instance_extensions = lx_null;
        g_instance_extensions_count = 0;
        g_instance_extensions_maxn = 0;
    }
    if (g_validation_layers) {
        lx_free(g_validation_layers);
        g_validation_layers = lx_null;
        g_validation_layers_count = 0;
        g_validation_layers_maxn = 0;
    }

}

VkPhysicalDevice lx_vk_physical_device_select(VkInstance instance) {

    // get device count
    lx_uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, lx_null);
    lx_assert_and_check_return_val(device_count, lx_null);

    // select a suitable device
    VkPhysicalDevice selected_device = lx_null;
    VkPhysicalDevice* devices = lx_nalloc0_type(device_count, VkPhysicalDevice);
    if (devices) {
        vkEnumeratePhysicalDevices(instance, &device_count, devices);
        lx_uint32_t i;
        for (i = 0; i < device_count; i++) {
            VkPhysicalDevice device = devices[i];
            if (device && lx_vk_device_is_suitable(device)) {
                selected_device = device;
                break;
            }
        }
        lx_free(devices);
    }
    return selected_device;
}

lx_int32_t lx_vk_physical_device_find_family_queue(VkPhysicalDevice device, VkQueueFlags queue_flags) {

    // get queue family count
    lx_uint32_t queuefamily_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamily_count, lx_null);
    lx_assert_and_check_return_val(queuefamily_count, lx_false);

    // has suitable queue family?
    lx_int32_t found = -1;
    VkQueueFamilyProperties* queuefamilies = lx_nalloc0_type(queuefamily_count, VkQueueFamilyProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamily_count, queuefamilies);
    if (queuefamilies) {
        lx_int32_t i;
        for (i = 0; i < queuefamily_count; i++) {
            VkQueueFamilyProperties* queuefamily = &queuefamilies[i];
            if (queuefamily->queueCount > 0 && queuefamily->queueFlags & queue_flags) {
                found = i;
                break;
            }
        }
        lx_free(queuefamilies);
    }
    return found;
}

VkDevice lx_vk_device_init_gpu_device(VkPhysicalDevice physical_device, VkQueue* pqueue, lx_uint32_t* pfamily_index) {
    lx_assert_and_check_return_val(physical_device, lx_null);

    // get graphics family queue index
    lx_int32_t family_index = lx_vk_physical_device_find_family_queue(physical_device, VK_QUEUE_GRAPHICS_BIT);
    lx_assert_and_check_return_val(family_index >= 0, lx_null);
    if (pfamily_index) {
        *pfamily_index = family_index;
    }

    // create device and get graphics queue
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_createinfo = {};
    queue_createinfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_createinfo.queueFamilyIndex = family_index;
    queue_createinfo.queueCount       = 1;
    queue_createinfo.pQueuePriorities = &queue_priority;

    VkDeviceCreateInfo createinfo = {};
    VkPhysicalDeviceFeatures device_features = {};
    createinfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createinfo.pQueueCreateInfos       = &queue_createinfo;
    createinfo.queueCreateInfoCount    = 1;
    createinfo.pEnabledFeatures        = &device_features;
    createinfo.ppEnabledExtensionNames = lx_vk_device_extensions(&createinfo.enabledExtensionCount);
    createinfo.ppEnabledLayerNames     = lx_vk_validation_layers(&createinfo.enabledLayerCount);

    VkDevice device = lx_null;
    if (vkCreateDevice(physical_device, &createinfo, lx_null, &device) == VK_SUCCESS) {
        if (pqueue) {
            vkGetDeviceQueue(device, family_index, 0, pqueue);
        }
    }
    return device;
}

lx_char_t const** lx_vk_instance_extensions(lx_uint32_t* pcount) {
    if (pcount) *pcount = g_instance_extensions_count;
    return (lx_char_t const**)g_instance_extensions;
}

lx_void_t lx_vk_instance_extensions_add(lx_char_t const** extensions, lx_uint32_t count) {
    if (extensions && count) {
        lx_uint32_t extensions_count = g_instance_extensions_count + count;
        if (!g_instance_extensions) {
            g_instance_extensions_maxn = extensions_count + 16;
            g_instance_extensions = lx_nalloc0_type(g_instance_extensions_maxn, lx_char_t*);
        } else if (extensions_count > g_instance_extensions_maxn) {
            g_instance_extensions_maxn = extensions_count + 16;
            g_instance_extensions = (lx_char_t**)lx_ralloc(g_instance_extensions, g_instance_extensions_maxn * sizeof(lx_char_t*));
        }
        if (g_instance_extensions) {
            lx_memcpy(g_instance_extensions + g_instance_extensions_count, extensions, count * sizeof(lx_char_t*));
            g_instance_extensions_count = extensions_count;
        }
    }
}

lx_bool_t lx_vk_instance_extensions_check(lx_char_t const** extensions, lx_uint32_t count) {
    lx_assert_and_check_return_val(extensions && count, lx_false);

    // get extension count
    lx_uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(lx_null, &extension_count, lx_null);
    lx_check_return_val(extension_count, lx_false);

    // check extension
    VkExtensionProperties* available_extensions = lx_nalloc0_type(extension_count, VkExtensionProperties);
    if (available_extensions) {
        vkEnumerateInstanceExtensionProperties(lx_null, &extension_count, available_extensions);
        lx_uint32_t i, j;
        lx_bool_t found = lx_false;
        for (i = 0; i < count; i++) {
            lx_char_t const* extensionname = extensions[i];
            if (extensionname) {
                for (j = 0; j < extension_count; j++) {
                    VkExtensionProperties* extension_properties = &available_extensions[j];
                    if (lx_strcmp(extensionname, extension_properties->extensionName) == 0) {
                        found = lx_true;
                        break;
                    }
                }
            }
            if (!found) {
                return lx_false;
            }
        }
        lx_free(available_extensions);
    }
    return lx_true;
}

lx_char_t const** lx_vk_device_extensions(lx_uint32_t* pcount) {
    if (pcount) *pcount = g_device_extensions_count;
    return (lx_char_t const**)g_device_extensions;
}

lx_void_t lx_vk_device_extensions_add(lx_char_t const** extensions, lx_uint32_t count) {
    if (extensions && count) {
        lx_uint32_t extensions_count = g_device_extensions_count + count;
        if (!g_device_extensions) {
            g_device_extensions_maxn = extensions_count + 16;
            g_device_extensions = lx_nalloc0_type(g_device_extensions_maxn, lx_char_t*);
        } else if (extensions_count > g_device_extensions_maxn) {
            g_device_extensions_maxn = extensions_count + 16;
            g_device_extensions = (lx_char_t**)lx_ralloc(g_device_extensions, g_device_extensions_maxn * sizeof(lx_char_t*));
        }
        if (g_device_extensions) {
            lx_memcpy(g_device_extensions + g_device_extensions_count, extensions, count * sizeof(lx_char_t*));
            g_device_extensions_count = extensions_count;
        }
    }
}

lx_bool_t lx_vk_device_extensions_check(VkPhysicalDevice device, lx_char_t const** extensions, lx_uint32_t count) {
    lx_assert_and_check_return_val(device && extensions && count, lx_false);

    // get extension count
    lx_uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, lx_null, &extension_count, lx_null);
    lx_check_return_val(extension_count, lx_false);

    // check extension
    VkExtensionProperties* available_extensions = lx_nalloc0_type(extension_count, VkExtensionProperties);
    if (available_extensions) {
        vkEnumerateDeviceExtensionProperties(device, lx_null, &extension_count, available_extensions);
        lx_uint32_t i, j;
        lx_bool_t found = lx_false;
        for (i = 0; i < count; i++) {
            lx_char_t const* extensionname = extensions[i];
            if (extensionname) {
                for (j = 0; j < extension_count; j++) {
                    VkExtensionProperties* extension_properties = &available_extensions[j];
                    if (lx_strcmp(extensionname, extension_properties->extensionName) == 0) {
                        found = lx_true;
                        break;
                    }
                }
            }
            if (!found) {
                return lx_false;
            }
        }
        lx_free(available_extensions);
    }
    return lx_true;
}

lx_char_t const** lx_vk_validation_layers(lx_uint32_t* pcount) {
    if (pcount) *pcount = g_validation_layers_count;
    return (lx_char_t const**)g_validation_layers;
}

lx_void_t lx_vk_validation_layers_add(lx_char_t const** validation_layers, lx_uint32_t count) {
    if (validation_layers && count) {
        lx_uint32_t validation_layers_count = g_validation_layers_count + count;
        if (!g_validation_layers) {
            g_validation_layers_maxn = validation_layers_count + 16;
            g_validation_layers = lx_nalloc0_type(g_validation_layers_maxn, lx_char_t*);
        } else if (validation_layers_count > g_validation_layers_maxn) {
            g_validation_layers_maxn = validation_layers_count + 16;
            g_validation_layers = (lx_char_t**)lx_ralloc(g_validation_layers, g_validation_layers_maxn * sizeof(lx_char_t*));
        }
        if (g_validation_layers) {
            lx_memcpy(g_validation_layers + g_validation_layers_count, validation_layers, count * sizeof(lx_char_t*));
            g_validation_layers_count = validation_layers_count;
        }
    }
}

lx_bool_t lx_vk_validation_layers_check(lx_char_t const** layers, lx_uint32_t count) {
    lx_assert_and_check_return_val(layers && count, lx_false);

    // get layer count
    lx_uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, lx_null);
    lx_check_return_val(layer_count, lx_false);

    // check layer
    VkLayerProperties* available_layers = lx_nalloc0_type(layer_count, VkLayerProperties);
    if (available_layers) {
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
        lx_uint32_t i, j;
        lx_bool_t found = lx_false;
        for (i = 0; i < count; i++) {
            lx_char_t const* layername = layers[i];
            if (layername) {
                for (j = 0; j < layer_count; j++) {
                    VkLayerProperties* layer_properties = &available_layers[j];
                    if (lx_strcmp(layername, layer_properties->layerName) == 0) {
                        found = lx_true;
                        break;
                    }
                }
            }
            if (!found) {
                return lx_false;
            }
        }
        lx_free(available_layers);
    }
    return lx_true;
}

lx_bool_t lx_vk_allocate_memory_type_from_properties(VkPhysicalDeviceMemoryProperties gpu_memory_properties,
    lx_uint32_t type_bits, VkFlags requirements_mask, lx_uint32_t* ptype_index) {
    for (lx_uint32_t i = 0; i < 32; i++) {
        if ((type_bits & 1) == 1) {
            if ((gpu_memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *ptype_index = i;
                return lx_true;
            }
        }
        type_bits >>= 1;
    }
    return lx_false;
}

lx_void_t lx_vk_set_image_layout(VkCommandBuffer cmdbuffer, VkImage image,
    VkImageLayout layout_old, VkImageLayout layout_new, VkPipelineStageFlags stages_src, VkPipelineStageFlags stages_dst) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = lx_null;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;
    barrier.oldLayout = layout_old;
    barrier.newLayout = layout_new;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    switch (layout_old) {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    default:
        break;
    }

    switch (layout_new) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    default:
        break;
    }
    vkCmdPipelineBarrier(cmdbuffer, stages_src, stages_dst, 0, 0, lx_null, 0, lx_null, 1, &barrier);
}

#ifdef LX_DEBUG
lx_void_t lx_vk_debug_messenger_setup(VkInstance instance, VkDebugUtilsMessengerEXT* pdebug_messenger) {
    PFN_vkCreateDebugUtilsMessengerEXT pvkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (pvkCreateDebugUtilsMessengerEXT) {
        lx_trace_d("setup debug messenger");
        VkDebugUtilsMessengerCreateInfoEXT createinfo = {};
        createinfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createinfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createinfo.pfnUserCallback = lx_vk_debug_utils_callback;
        if (pvkCreateDebugUtilsMessengerEXT(instance, &createinfo, lx_null, pdebug_messenger) != VK_SUCCESS) {
            lx_trace_e("failed to setup debug messenger!");
        }
    }
}

lx_void_t lx_vk_debug_messenger_cancel(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger) {
    PFN_vkDestroyDebugUtilsMessengerEXT pvkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (pvkDestroyDebugUtilsMessengerEXT) {
        pvkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, lx_null);
    }
}

lx_void_t lx_vk_debug_report_setup(VkInstance instance, VkDebugReportCallbackEXT* pdebug_report_cb) {
    PFN_vkCreateDebugReportCallbackEXT pvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (pvkCreateDebugReportCallbackEXT) {
        lx_trace_d("setup debug report");
        VkDebugReportCallbackCreateInfoEXT createinfo = {};
        createinfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createinfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createinfo.pfnCallback = lx_vk_debug_report_callback;
        createinfo.pUserData   = lx_null;
        if (pvkCreateDebugReportCallbackEXT(instance, &createinfo, lx_null, pdebug_report_cb) != VK_SUCCESS) {
            lx_trace_e("failed to setup debug report callback!");
        }
    }
}

lx_void_t lx_vk_debug_report_cancel(VkInstance instance, VkDebugReportCallbackEXT debug_report_cb) {
    PFN_vkDestroyDebugReportCallbackEXT pvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (pvkDestroyDebugReportCallbackEXT) {
        pvkDestroyDebugReportCallbackEXT(instance, debug_report_cb, lx_null);
    }
}
#endif
