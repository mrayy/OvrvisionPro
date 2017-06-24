// ovrvision_csharp.cpp
//
//MIT License
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
//
// Oculus Rift : TM & Copyright Oculus VR, Inc. All Rights Reserved
// Unity : TM & Copyright Unity Technologies. All Rights Reserved

/////////// INCLUDE ///////////

// Which graphics device APIs we possibly support?
#ifdef WIN32
#define SUPPORT_D3D9 1
#define SUPPORT_D3D11 1 // comment this out if you don't have D3D11 header/library files
#define SUPPORT_D3D12 0
#define SUPPORT_OPENGL 1
#endif

#ifdef MACOSX
#define SUPPORT_OPENGL 1
#endif

#ifdef LINUX
#define SUPPORT_OPENGL 1
#endif

#if SUPPORT_D3D9
#include <d3d9.h>
#endif
#if SUPPORT_D3D11
#include <d3d11.h>
#endif
#if SUPPORT_D3D12
#include <d3d12.h>
#endif
#if SUPPORT_OPENGL
#if WIN32
#include <gl/GL.h>
#elif MACOSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glx.h>
#endif
#endif

#include "IUnityInterface.h"

/////////// VARS AND DEFS ///////////

//Exporter
#ifdef WIN32
#define CSHARP_EXPORT __declspec(dllexport)
#elif MACOSX
#define CSHARP_EXPORT 
#else
#define CSHARP_EXPORT
#endif

// Event types for UnitySetGraphicsDevice
enum GfxDeviceEventType {
	kGfxDeviceEventInitialize = 0,
	kGfxDeviceEventShutdown,
	kGfxDeviceEventBeforeReset,
	kGfxDeviceEventAfterReset,
};

/////////// EXPORT FUNCTION ///////////

//C language
#ifdef __cplusplus
extern "C" {
#endif

//Global var
float g_Time;

void CSHARP_EXPORT SetTimeFromUnity(float t)
{
	g_Time = t;
}

#ifdef __cplusplus
}
#endif
