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
 * @file        vk.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_VK_H
#define LX_CORE_DEVICE_VULKAN_VK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../path.h"
#include "../../paint.h"
#include "../../private/stroker.h"
#include "../../../platform/window.h"
#ifdef LX_CONFIG_OS_ANDROID
#   define VK_USE_PLATFORM_ANDROID_KHR
#   define VK_NO_PROTOTYPES 1
#endif
#include <vulkan/vulkan.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef VK_NO_PROTOTYPES
#   define LX_VK_API_EXTERN(name) extern PFN_##name name;
#else
#   define LX_VK_API_EXTERN(name);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the swapchain type
typedef struct lx_vk_swapchain_t_ {
    VkSwapchainKHR  swapchain;
    VkFormat        format;
} lx_vk_swapchain_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init vulkan context
 *
 * @return                      lx_true or lx_false
 */
lx_bool_t                       lx_vk_context_init(lx_noarg_t);

/* exit vulkan context
 */
lx_void_t                       lx_vk_context_exit(lx_noarg_t);

/* select suitable vulkan physical device
 *
 * @param instance              the vulkan instance
 *
 * @return                      the vulkan device
 */
VkPhysicalDevice                lx_vk_physical_device_select(VkInstance instance);

/* find family queue from the physical device
 *
 * @param device                the physical device
 * @param queue_flags           the queue flags
 *
 * @return                      the family queue index, -1: not found
 */
lx_int32_t                      lx_vk_physical_device_find_family_queue(VkPhysicalDevice device, VkQueueFlags queue_flags);

/* create gpu device and queue (logical device)
 *
 * @param physical_device       the vulkan physical device
 * @param pqueue                the queue pointer
 *
 * @return                      the logical device
 */
VkDevice                        lx_vk_device_init_gpu_device(VkPhysicalDevice physical_device, VkQueue* pqueue);

/* get the enabled instance extensions
 *
 * @param pcount                the extensions count pointer
 *
 * @return                      the enabled extensions
 */
lx_char_t const**               lx_vk_instance_extensions(lx_uint32_t* pcount);

/* add the enabled instance extensions
 *
 * @param extensions            the enabled extensions
 * @param count                 the extensions count
 */
lx_void_t                       lx_vk_instance_extensions_add(lx_char_t const** extensions, lx_uint32_t count);

/* check the support instance extensions
 *
 * @param extensions            the checked extensions
 * @param count                 the extensions count
 *
 * @return                      lx_true or lx_false
 */
lx_bool_t                       lx_vk_instance_extensions_check(lx_char_t const** extensions, lx_uint32_t count);

/* get the enabled device extensions
 *
 * @param pcount                the extensions count pointer
 *
 * @return                      the enabled extensions
 */
lx_char_t const**               lx_vk_device_extensions(lx_uint32_t* pcount);

/* add the enabled device extensions
 *
 * @param extensions            the enabled extensions
 * @param count                 the extensions count
 */
lx_void_t                       lx_vk_device_extensions_add(lx_char_t const** extensions, lx_uint32_t count);

/* check the support device extensions
 *
 * @param device                the physical device
 * @param extensions            the checked extensions
 * @param count                 the extensions count
 *
 * @return                      lx_true or lx_false
 */
lx_bool_t                       lx_vk_device_extensions_check(VkPhysicalDevice device, lx_char_t const** extensions, lx_uint32_t count);


/* get the enabled validation layers
 *
 * @param pcount                the validation layers count pointer
 *
 * @return                      the enabled validation layers
 */
lx_char_t const**               lx_vk_validation_layers(lx_uint32_t* pcount);

/* add the enabled validation layers
 *
 * @param validation_layers     the enabled validation layers
 * @param count                 the validation layers count
 */
lx_void_t                       lx_vk_validation_layers_add(lx_char_t const** validation_layers, lx_uint32_t count);

/* check the support validation layers
 *
 * @param layers                the checked layers
 * @param count                 the layers count
 *
 * @return                      lx_true or lx_false
 */
lx_bool_t                       lx_vk_validation_layers_check(lx_char_t const** layers, lx_uint32_t count);

/* setup debug messenger
 *
 * @param instance              the vulkan instance
 * @param pdebug_messenger      the debug messenger pointer
 */
lx_void_t                       lx_vk_debug_messenger_setup(VkInstance instance, VkDebugUtilsMessengerEXT* pdebug_messenger);

/* cancel debug messenger
 *
 * @param instance              the vulkan instance
 * @param debug_messenger       the debug messenger
 */
lx_void_t                       lx_vk_debug_messenger_cancel(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger);

/* setup debug report callback
 *
 * @param instance              the vulkan instance
 * @param pdebug_report_cb      the debug report callback pointer
 */
lx_void_t                       lx_vk_debug_report_setup(VkInstance instance, VkDebugReportCallbackEXT* pdebug_report_cb);

/* cancel debug report callback
 *
 * @param instance              the vulkan instance
 * @param debug_report_cb       the debug report callback
 */
lx_void_t                       lx_vk_debug_report_cancel(VkInstance instance, VkDebugReportCallbackEXT debug_report_cb);

/* init swapchain
 *
 * @param swapchain             the swapchain
 * @param gpu_device            the gpu device
 * @param surface               the vulkan surface
 *
 * @return                      lx_true or lx_false
 */
lx_bool_t                       lx_vk_swapchain_init(lx_vk_swapchain_t* swapchain, VkPhysicalDevice gpu_device, VkSurfaceKHR surface);

/* exit swapchain
 *
 * @param swapchain             the swapchain
 */
lx_void_t                       lx_vk_swapchain_exit(lx_vk_swapchain_t* swapchain);

/* //////////////////////////////////////////////////////////////////////////////////////
 * vulkan interfaces
 */
LX_VK_API_EXTERN(vkCreateInstance);
LX_VK_API_EXTERN(vkDestroyInstance);
LX_VK_API_EXTERN(vkEnumeratePhysicalDevices);
LX_VK_API_EXTERN(vkGetPhysicalDeviceFeatures);
LX_VK_API_EXTERN(vkGetPhysicalDeviceFormatProperties);
LX_VK_API_EXTERN(vkGetPhysicalDeviceImageFormatProperties);
LX_VK_API_EXTERN(vkGetPhysicalDeviceProperties);
LX_VK_API_EXTERN(vkGetPhysicalDeviceQueueFamilyProperties);
LX_VK_API_EXTERN(vkGetPhysicalDeviceMemoryProperties);
LX_VK_API_EXTERN(vkGetInstanceProcAddr);
LX_VK_API_EXTERN(vkGetDeviceProcAddr);
LX_VK_API_EXTERN(vkCreateDevice);
LX_VK_API_EXTERN(vkDestroyDevice);
LX_VK_API_EXTERN(vkEnumerateInstanceExtensionProperties);
LX_VK_API_EXTERN(vkEnumerateDeviceExtensionProperties);
LX_VK_API_EXTERN(vkEnumerateInstanceLayerProperties);
LX_VK_API_EXTERN(vkEnumerateDeviceLayerProperties);
LX_VK_API_EXTERN(vkGetDeviceQueue);
LX_VK_API_EXTERN(vkQueueSubmit);
LX_VK_API_EXTERN(vkQueueWaitIdle);
LX_VK_API_EXTERN(vkDeviceWaitIdle);
LX_VK_API_EXTERN(vkAllocateMemory);
LX_VK_API_EXTERN(vkFreeMemory);
LX_VK_API_EXTERN(vkMapMemory);
LX_VK_API_EXTERN(vkUnmapMemory);
LX_VK_API_EXTERN(vkFlushMappedMemoryRanges);
LX_VK_API_EXTERN(vkInvalidateMappedMemoryRanges);
LX_VK_API_EXTERN(vkGetDeviceMemoryCommitment);
LX_VK_API_EXTERN(vkBindBufferMemory);
LX_VK_API_EXTERN(vkBindImageMemory);
LX_VK_API_EXTERN(vkGetBufferMemoryRequirements);
LX_VK_API_EXTERN(vkGetImageMemoryRequirements);
LX_VK_API_EXTERN(vkGetImageSparseMemoryRequirements);
LX_VK_API_EXTERN(vkGetPhysicalDeviceSparseImageFormatProperties);
LX_VK_API_EXTERN(vkQueueBindSparse);
LX_VK_API_EXTERN(vkCreateFence);
LX_VK_API_EXTERN(vkDestroyFence);
LX_VK_API_EXTERN(vkResetFences);
LX_VK_API_EXTERN(vkGetFenceStatus);
LX_VK_API_EXTERN(vkWaitForFences);
LX_VK_API_EXTERN(vkCreateSemaphore);
LX_VK_API_EXTERN(vkDestroySemaphore);
LX_VK_API_EXTERN(vkCreateEvent);
LX_VK_API_EXTERN(vkDestroyEvent);
LX_VK_API_EXTERN(vkGetEventStatus);
LX_VK_API_EXTERN(vkSetEvent);
LX_VK_API_EXTERN(vkResetEvent);
LX_VK_API_EXTERN(vkCreateQueryPool);
LX_VK_API_EXTERN(vkDestroyQueryPool);
LX_VK_API_EXTERN(vkGetQueryPoolResults);
LX_VK_API_EXTERN(vkCreateBuffer);
LX_VK_API_EXTERN(vkDestroyBuffer);
LX_VK_API_EXTERN(vkCreateBufferView);
LX_VK_API_EXTERN(vkDestroyBufferView);
LX_VK_API_EXTERN(vkCreateImage);
LX_VK_API_EXTERN(vkDestroyImage);
LX_VK_API_EXTERN(vkGetImageSubresourceLayout);
LX_VK_API_EXTERN(vkCreateImageView);
LX_VK_API_EXTERN(vkDestroyImageView);
LX_VK_API_EXTERN(vkCreateShaderModule);
LX_VK_API_EXTERN(vkDestroyShaderModule);
LX_VK_API_EXTERN(vkCreatePipelineCache);
LX_VK_API_EXTERN(vkDestroyPipelineCache);
LX_VK_API_EXTERN(vkGetPipelineCacheData);
LX_VK_API_EXTERN(vkMergePipelineCaches);
LX_VK_API_EXTERN(vkCreateGraphicsPipelines);
LX_VK_API_EXTERN(vkCreateComputePipelines);
LX_VK_API_EXTERN(vkDestroyPipeline);
LX_VK_API_EXTERN(vkCreatePipelineLayout);
LX_VK_API_EXTERN(vkDestroyPipelineLayout);
LX_VK_API_EXTERN(vkCreateSampler);
LX_VK_API_EXTERN(vkDestroySampler);
LX_VK_API_EXTERN(vkCreateDescriptorSetLayout);
LX_VK_API_EXTERN(vkDestroyDescriptorSetLayout);
LX_VK_API_EXTERN(vkCreateDescriptorPool);
LX_VK_API_EXTERN(vkDestroyDescriptorPool);
LX_VK_API_EXTERN(vkResetDescriptorPool);
LX_VK_API_EXTERN(vkAllocateDescriptorSets);
LX_VK_API_EXTERN(vkFreeDescriptorSets);
LX_VK_API_EXTERN(vkUpdateDescriptorSets);
LX_VK_API_EXTERN(vkCreateFramebuffer);
LX_VK_API_EXTERN(vkDestroyFramebuffer);
LX_VK_API_EXTERN(vkCreateRenderPass);
LX_VK_API_EXTERN(vkDestroyRenderPass);
LX_VK_API_EXTERN(vkGetRenderAreaGranularity);
LX_VK_API_EXTERN(vkCreateCommandPool);
LX_VK_API_EXTERN(vkDestroyCommandPool);
LX_VK_API_EXTERN(vkResetCommandPool);
LX_VK_API_EXTERN(vkAllocateCommandBuffers);
LX_VK_API_EXTERN(vkFreeCommandBuffers);
LX_VK_API_EXTERN(vkBeginCommandBuffer);
LX_VK_API_EXTERN(vkEndCommandBuffer);
LX_VK_API_EXTERN(vkResetCommandBuffer);
LX_VK_API_EXTERN(vkCmdBindPipeline);
LX_VK_API_EXTERN(vkCmdSetViewport);
LX_VK_API_EXTERN(vkCmdSetScissor);
LX_VK_API_EXTERN(vkCmdSetLineWidth);
LX_VK_API_EXTERN(vkCmdSetDepthBias);
LX_VK_API_EXTERN(vkCmdSetBlendConstants);
LX_VK_API_EXTERN(vkCmdSetDepthBounds);
LX_VK_API_EXTERN(vkCmdSetStencilCompareMask);
LX_VK_API_EXTERN(vkCmdSetStencilWriteMask);
LX_VK_API_EXTERN(vkCmdSetStencilReference);
LX_VK_API_EXTERN(vkCmdBindDescriptorSets);
LX_VK_API_EXTERN(vkCmdBindIndexBuffer);
LX_VK_API_EXTERN(vkCmdBindVertexBuffers);
LX_VK_API_EXTERN(vkCmdDraw);
LX_VK_API_EXTERN(vkCmdDrawIndexed);
LX_VK_API_EXTERN(vkCmdDrawIndirect);
LX_VK_API_EXTERN(vkCmdDrawIndexedIndirect);
LX_VK_API_EXTERN(vkCmdDispatch);
LX_VK_API_EXTERN(vkCmdDispatchIndirect);
LX_VK_API_EXTERN(vkCmdCopyBuffer);
LX_VK_API_EXTERN(vkCmdCopyImage);
LX_VK_API_EXTERN(vkCmdBlitImage);
LX_VK_API_EXTERN(vkCmdCopyBufferToImage);
LX_VK_API_EXTERN(vkCmdCopyImageToBuffer);
LX_VK_API_EXTERN(vkCmdUpdateBuffer);
LX_VK_API_EXTERN(vkCmdFillBuffer);
LX_VK_API_EXTERN(vkCmdClearColorImage);
LX_VK_API_EXTERN(vkCmdClearDepthStencilImage);
LX_VK_API_EXTERN(vkCmdClearAttachments);
LX_VK_API_EXTERN(vkCmdResolveImage);
LX_VK_API_EXTERN(vkCmdSetEvent);
LX_VK_API_EXTERN(vkCmdResetEvent);
LX_VK_API_EXTERN(vkCmdWaitEvents);
LX_VK_API_EXTERN(vkCmdPipelineBarrier);
LX_VK_API_EXTERN(vkCmdBeginQuery);
LX_VK_API_EXTERN(vkCmdEndQuery);
LX_VK_API_EXTERN(vkCmdResetQueryPool);
LX_VK_API_EXTERN(vkCmdWriteTimestamp);
LX_VK_API_EXTERN(vkCmdCopyQueryPoolResults);
LX_VK_API_EXTERN(vkCmdPushConstants);
LX_VK_API_EXTERN(vkCmdBeginRenderPass);
LX_VK_API_EXTERN(vkCmdNextSubpass);
LX_VK_API_EXTERN(vkCmdEndRenderPass);
LX_VK_API_EXTERN(vkCmdExecuteCommands);

// VK_KHR_surface
LX_VK_API_EXTERN(vkDestroySurfaceKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceSurfaceSupportKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceSurfaceFormatsKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceSurfacePresentModesKHR);

// VK_KHR_swapchain
LX_VK_API_EXTERN(vkCreateSwapchainKHR);
LX_VK_API_EXTERN(vkDestroySwapchainKHR);
LX_VK_API_EXTERN(vkGetSwapchainImagesKHR);
LX_VK_API_EXTERN(vkAcquireNextImageKHR);
LX_VK_API_EXTERN(vkQueuePresentKHR);

// VK_KHR_display
LX_VK_API_EXTERN(vkGetPhysicalDeviceDisplayPropertiesKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
LX_VK_API_EXTERN(vkGetDisplayPlaneSupportedDisplaysKHR);
LX_VK_API_EXTERN(vkGetDisplayModePropertiesKHR);
LX_VK_API_EXTERN(vkCreateDisplayModeKHR);
LX_VK_API_EXTERN(vkGetDisplayPlaneCapabilitiesKHR);
LX_VK_API_EXTERN(vkCreateDisplayPlaneSurfaceKHR);

// VK_KHR_display_swapchain
LX_VK_API_EXTERN(vkCreateSharedSwapchainsKHR);

#ifdef VK_USE_PLATFORM_XLIB_KHR
// VK_KHR_xlib_surface
LX_VK_API_EXTERN(vkCreateXlibSurfaceKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
// VK_KHR_xcb_surface
LX_VK_API_EXTERN(vkCreateXcbSurfaceKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
// VK_KHR_wayland_surface
LX_VK_API_EXTERN(vkCreateWaylandSurfaceKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
// VK_KHR_mir_surface
LX_VK_API_EXTERN(vkCreateMirSurfaceKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceMirPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
// VK_KHR_android_surface
LX_VK_API_EXTERN(vkCreateAndroidSurfaceKHR);
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
// VK_KHR_win32_surface
LX_VK_API_EXTERN(vkCreateWin32SurfaceKHR);
LX_VK_API_EXTERN(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


