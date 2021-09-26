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
#   define LX_VK_API_DEFINE(name) extern PFN_##name name;
#else
#   define LX_VK_API_DEFINE(name);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init vulkan context
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vk_context_init(lx_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * vulkan interfaces
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
 * extern
 */
lx_extern_c_leave

#endif


