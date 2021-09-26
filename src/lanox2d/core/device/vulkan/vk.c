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
#   define LX_VK_API_LOAD(name) do {\
    name = (PFN_##name)lx_dlsym(s_library, #name);\
    lx_assert_and_check_return_val(name, lx_false);\
} while (0)
#else
#   define LX_VK_API_LOAD(name);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_vk_context_init() {
    // load symbols from vulkan.so
    static lx_dlimage_ref_t s_library = lx_null;
    if (!s_library) {
        s_library = lx_dlopen("vulkan.so", LX_RTLD_LAZY);
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
    return lx_true;
}

