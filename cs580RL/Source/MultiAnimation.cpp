///--------------------------------------------------------------------------------------
// File: MultiAnimation.cpp
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#pragma warning(disable: 4995)
#include "resource.h"
#include "DXUT\DXUTcamera.h"
#include "DXUT\DXUTsettingsdlg.h"
#include "DXUT\SDKmisc.h"
#include "DXUT\SDKsound.h"
#include "MultiAnimation.h"
#include "Tiny.h"
#include "world.h"
#include "gameobject.h"
#include "body.h"
#include "statemch.h"
#include "database.h"
#include "terrain.h"
#include "global.h"
#include "RLGame.h"
#pragma warning(default: 4995)


using namespace std;

//#define OLD_TINY

#define TXFILE_FLOOR L"floor.jpg"
#define FLOOR_TILECOUNT 2

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
CFirstPersonCamera      g_Camera;               // A model viewing camera
CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls
ID3DXMesh*              g_pMeshFloor = NULL;    // floor geometry
D3DXMATRIXA16           g_mxFloor;              // floor world xform
D3DMATERIAL9            g_MatFloor;             // floor material
IDirect3DTexture9*      g_pTxFloor = NULL;      // floor texture
CSoundManager           g_DSound;               // DirectSound class
CMultiAnim              g_MultiAnim;            // the MultiAnim class for holding Tiny's mesh and frame hierarchy
vector< CTiny* >        g_v_pCharacters;        // array of character objects; each can be associated with any of the CMultiAnims
DWORD                   g_dwFollow = 0xffffffff;// which character the camera should follow; 0xffffffff for static camera
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
bool                    g_bPlaySounds = true;   // whether to play sounds
double                  g_fLastAnimTime = 0.0;  // Time for the animations

World*					g_pWorld = NULL;		// World object

bool					g_frontCam = false;		// Default set to front camera
float					g_heuristicWeight = 1.0f;// Default heuristic weight for A*
int						g_heuristicCalc = 0;	// Default heuristic calc for A*
bool					g_smoothing = false;	//Default smoothing
bool					g_rubberbanding = false;//Default rubberbanding
bool					g_straightline = false;	//Default straight line optimization
int						g_animStyle = 1;		//Default anim style
bool					g_singleStep = true;	//Default single step
bool					g_aStarUsesAnalysis = false;//Default A* uses analysis
bool                    g_movementFlag = true;//Agent will move by default
bool                    g_fogOfWarFlag = false;//Agent has Fog of War disabled by default

float					g_punish        = 10.0f;                // Default punishment
float					g_reward        = 50.0f;                // Default reward
int						g_trainloop     = 1000;		            // Default loop time
bool					g_useQR         = true;			        // Default use Q-Learing
RLSpeed					g_RLspeed       = RLSpeed::Medium;      // Default speed(single step)
int						g_cureIteration = 0;					// Current iteration
unsigned int			g_trainingStatus= 0;					// Training status info(such as waiting, training, complete, get from somewhere)

unsigned int			g_catWin = 0;				
unsigned int			g_mouseWin = 0;
unsigned int			g_trainCatWin = 0;
unsigned int			g_trainMouseWin = 0;

bool					g_playContinuous = false;
bool					g_useSmartMouse = false;

D3DXVECTOR3				g_click2D;
D3DXVECTOR3				g_click3D;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_ADDTINY             5
#define IDC_NEXTVIEW            6
#define IDC_PREVVIEW            7
#define IDC_ENABLESOUND         8
#define IDC_CONTROLTINY         9
#define IDC_RELEASEALL          10
#define IDC_RESETCAMERA         11
#define IDC_RESETTIME           12
#define IDC_TOGGLECAM			14
#define IDC_TOGGLEHEURISTICWEIGHT	15
#define IDC_TOGGLEHEURISTIC		16
#define IDC_TOGGLESMOOTHING		17
#define IDC_TOGGLERUBBERBANDING 18
#define IDC_TOGGLESTRAIGHTLINE  19
#define IDC_TOGGLEANALYSIS      20
#define IDC_TOGGLEASTARUSESANALYSIS	21
#define IDC_TOGGLEAGENTSPEED	22
#define IDC_TOGGLESINGLESTEP	23
#define IDC_TOGGLEMOVEMENT      24
#define IDC_RUNTIMINGSSHORT     25
#define IDC_RUNTIMINGSLONG      26
#define IDC_TOGGLEFOW           27

#define IDC_NEXTMAP             13
#define IDC_PUNISH 28
#define IDC_REWARD 29
#define IDC_LOOP_1000 30
#define IDC_LOOP_5000 31
#define IDC_LOOP_10000 32
#define IDC_LOOP_50000 33
#define IDC_METHOD_QL 34
#define IDC_METHOD_SARSA 35
#define IDC_START_TRAINING 36
#define IDC_RESET_RL 37
#define IDC_START_PLAYING 38
#define IDC_SPEED_SUPERSLOW 39
#define IDC_SPEED_SLOW 40
#define IDC_SPEED_MEDIUM 41
#define IDC_SPEED_FAST 42
#define IDC_SPEED_TURBO 43
#define IDC_CLEAR_SCORE 44
#define IDC_TOOGLE_CONTINUOUS 45
#define IDC_TOOGLE_SMART_MOUSE 46
#define IDC_TOOGLE_GREEDY_MOUSE 47

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );

void    InitApp();
void	RedrawButtons();

HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions. These functions allow DXUT to notify
    // the application about device changes, user input, and windows messages.  The
    // callbacks are optional so you need only set callbacks for events you're interested
    // in. However, if you don't handle the device reset/lost callbacks then the sample
    // framework won't be able to reset your device since the application must first
    // release all device resources before resetting.  Likewise, if you don't handle the
    // device created/destroyed callbacks then DXUT won't be able to
    // recreate your device resources.
    DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
	DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    

    // Initialize DXUT and create the desired Win32 window and Direct3D
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
	DXUTCreateWindow( L"CS380/CS580 Project 2: A* Pathfinding" );
	InitApp();
    // We need to set up DirectSound after we have a window.
    g_DSound.Initialize( DXUTGetHWND(), DSSCL_PRIORITY );

    DXUTCreateDevice( true, 640, 480 );

    // Pass control to DXUT for handling the message pump and
    // dispatching render calls. DXUT will call your FrameMove
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

#ifdef OLD_TINY
    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.
    vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
    for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
    {
        ( * itCurCP )->Cleanup();
        delete * itCurCP;
    }
    g_v_pCharacters.clear();
#endif

    delete g_apSoundsTiny[ 0 ];
    delete g_apSoundsTiny[ 1 ];

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
	LPRECT winRect = new RECT;
	GetWindowRect(DXUTGetHWND(), winRect);

	long winMidVer = (winRect->right + winRect->left) / 2 - winRect->left;
    // Initialize dialogs
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    // g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    //g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    //g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;

	
 /* g_SampleUI.AddButton( IDC_NEXTMAP, L"Next Map", 45, iY, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLECAM, L"Toggle Camera", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEHEURISTICWEIGHT, L"Toggle Weight", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEHEURISTIC, L"Toggle Heuristic", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLESMOOTHING, L"Toggle Smoothing", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLERUBBERBANDING, L"Toggle Rubberbanding", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLESTRAIGHTLINE, L"Toggle Straight Line", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEANALYSIS, L"Toggle Analysis", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEASTARUSESANALYSIS, L"Toggle A* w/analysis", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEAGENTSPEED, L"Toggle Jog/Walk", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLESINGLESTEP, L"Toggle Single Step", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEMOVEMENT, L"Toggle Movement", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_TOGGLEFOW, L"Fog of War", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_RUNTIMINGSSHORT, L"Timing Test Short", 45, iY += 26, 120, 24 );
    g_SampleUI.AddButton( IDC_RUNTIMINGSLONG, L"Timing Test Long", 45, iY += 26, 120, 24 );	
	*/
	
	g_SampleUI.AddButton(IDC_PUNISH,L"Punish",40,iY,60,24);
	g_SampleUI.AddButton(IDC_REWARD,L"Reward",100, iY, 60, 24);

	g_SampleUI.AddButton(IDC_LOOP_1000, L"1000", 40, iY+=52, 60, 24);
	g_SampleUI.AddButton(IDC_LOOP_5000, L"5000", 100, iY, 60, 24);

	g_SampleUI.AddButton(IDC_LOOP_10000, L"10000", 40, iY += 26, 60, 24);
	g_SampleUI.AddButton(IDC_LOOP_50000, L"50000", 100, iY, 60, 24);

	g_SampleUI.AddButton(IDC_METHOD_QL, L"Q-Learning", 40, iY += 52, 60, 24);
	g_SampleUI.AddButton(IDC_METHOD_SARSA, L"SARSA", 100, iY, 60, 24);

	g_SampleUI.AddButton(IDC_START_TRAINING, L"Start/stop training", 40, iY += 52, 120, 48);

	g_SampleUI.AddButton(IDC_RESET_RL, L"Reset", 40, iY += 100, 120, 48);
	g_SampleUI.AddButton(IDC_START_PLAYING, L"Start/stop playing", 40, iY += 52, 120, 48);

	g_SampleUI.AddButton(IDC_SPEED_SUPERSLOW, L"Super slow", -(winMidVer + 30), iY += 140, 70, 16);
	g_SampleUI.AddButton(IDC_SPEED_SLOW, L"Slow", -(winMidVer -60), iY, 70, 16);
	g_SampleUI.AddButton(IDC_SPEED_MEDIUM, L"Medium", -(winMidVer -150), iY, 70, 16);
	g_SampleUI.AddButton(IDC_SPEED_FAST, L"Fast", -(winMidVer -240), iY, 70, 16);
	g_SampleUI.AddButton(IDC_SPEED_TURBO, L"Turbo", -(winMidVer - 330), iY, 70, 16);

    // Add mixed vp to the available vp choices in device settings dialog.
    DXUTGetD3D9Enumeration()->SetPossibleVertexProcessingList( true, false, false, true );
    
	// Create the World
	g_pWorld = new World();
	g_pWorld->InitializeSingletons();

    // Setup the camera with view matrix
	D3DXVECTOR3 vEye( .5f, 1.0f, .4f );
	D3DXVECTOR3 vAt( .5f,  -.10f, .5f );
    g_Camera.SetViewParams( &vEye, &vAt );
    g_Camera.SetScalers( 0.01f, 1.0f );  // Camera movement parameters
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Need to support ps 1.1
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 1, 1 ) )
        return false;

    // Need to support A8R8G8B8 textures
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, 0,
                                         D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8 ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

     // If the hardware cannot do vertex blending, use software vertex processing.
    if( pCaps->MaxVertexBlendMatrices < 2 )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // If using hardware vertex processing, change to mixed vertex processing
    // so there is a fallback.
    if( pDeviceSettings->d3d9.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

    // Debugging vertex shaders requires either REF or software vertex processing
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning(  pDeviceSettings->ver  );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;


    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                         L"Arial", &g_pFont ) );

    // Initialize floor textures
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, TXFILE_FLOOR ) );
    V_RETURN( D3DXCreateTextureFromFile( pd3dDevice, str, &g_pTxFloor ) );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the
    // shader debugger. Debugging vertex shaders requires either REF or software vertex
    // processing, and debugging pixel shaders requires REF.  The
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the
    // shader debugger.  It enables source level debugging, prevents instruction
    // reordering, prevents dead code elimination, and forces the compiler to compile
    // against the next higher available software target, which ensures that the
    // unoptimized shaders do not exceed the shader model limitations.  Setting these
    // flags will cause slower rendering since the shaders will be unoptimized and
    // forced into software.  See the DirectX documentation for more information about
    // using the shader debugger.
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Read the D3DX effect file
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"MultiAnimation.fx" ) );

    // If this fails, there should be debug output as to
    // they the .fx file failed to compile
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags,
                                        NULL, &g_pEffect, NULL ) );
    g_pEffect->SetTechnique( "RenderScene" );

    D3DXMATRIX mx;
    // floor geometry transform
    D3DXMatrixRotationX( & g_mxFloor, -D3DX_PI / 2.0f );
    D3DXMatrixRotationY( & mx, D3DX_PI / 4.0f );
    D3DXMatrixMultiply( & g_mxFloor, & g_mxFloor, & mx );
    D3DXMatrixTranslation( & mx, 0.5f, 0.0f, 0.5f );
    D3DXMatrixMultiply( & g_mxFloor, & g_mxFloor, & mx );

    // set material for floor
    g_MatFloor.Diffuse = D3DXCOLOR( 1.f, 1.f, 1.f, .75f );
    g_MatFloor.Ambient = D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
    g_MatFloor.Specular = D3DXCOLOR( 0.f, 0.f, 0.f, 1.f );
    g_MatFloor.Emissive = D3DXCOLOR( .0f, 0.f, 0.f, 0.f );
    g_MatFloor.Power = 0.f;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );

    // set up MultiAnim
    WCHAR sXFile[MAX_PATH];
    WCHAR str[MAX_PATH];

    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"MultiAnimation.fx" ) );
    V_RETURN( DXUTFindDXSDKMediaFileCch( sXFile, MAX_PATH, L"tiny_4anim.x" ) );

    CMultiAnimAllocateHierarchy AH;
    AH.SetMA( & g_MultiAnim );

    V_RETURN( g_MultiAnim.Setup( pd3dDevice, sXFile, str, &AH ) );

    // get device caps
    D3DCAPS9 caps;
    pd3dDevice->GetDeviceCaps( & caps );

    // Select the technique that fits the shader version.
    // We could have used ValidateTechnique()/GetNextValidTechnique() to find the
    // best one, but this is convenient for our purposes.
    if( caps.VertexShaderVersion >= D3DVS_VERSION( 2, 0 ) )
        g_MultiAnim.SetTechnique( "Skinning20" );
    else
        g_MultiAnim.SetTechnique( "Skinning11" );

#ifdef OLD_TINY
    // Restore steps for tiny instances
    vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
    for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
    {
        ( * itCurCP )->RestoreDeviceObjects( pd3dDevice );
    }
#endif

	g_pWorld->RestoreDeviceObjects( pd3dDevice );

	//Initialize world if needed
	g_pWorld->Initialize( & g_MultiAnim, & g_v_pCharacters, & g_DSound, DXUTGetGlobalTimer()->GetTime() );



#ifdef OLD_TINY
    // If there is no instance, make sure we have at least one.
    if( g_v_pCharacters.size() == 0 )
    {
        CTiny * pTiny = new CTiny;
        if( pTiny == NULL )
            return E_OUTOFMEMORY;

        hr = pTiny->Setup( & g_MultiAnim, & g_v_pCharacters, & g_DSound, 0.f );
        pTiny->SetSounds( g_bPlaySounds );
    }
#endif

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
    if( g_pEffect )
        V_RETURN( g_pEffect->OnResetDevice() );
    ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
    if( pMAEffect )
    {
        pMAEffect->OnResetDevice();
        pMAEffect->Release();
    }

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/3, fAspectRatio, 0.001f, 100.0f );

    // set lighting
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
    pd3dDevice->LightEnable( 0, TRUE );
    pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

    // create the floor geometry
    LPD3DXMESH pMesh;
    V_RETURN( D3DXCreatePolygon( pd3dDevice, 1.2f, 4, & pMesh, NULL ) );
    V_RETURN( pMesh->CloneMeshFVF( D3DXMESH_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, pd3dDevice, & g_pMeshFloor ) );
    SAFE_RELEASE( pMesh );

    DWORD dwNumVx = g_pMeshFloor->GetNumVertices();

    struct Vx
    {
        D3DXVECTOR3 vPos;
        D3DXVECTOR3 vNorm;
        float fTex[ 2 ];
    };

    // Initialize its texture coordinates
    Vx * pVx;
    hr = g_pMeshFloor->LockVertexBuffer( 0, (VOID **) & pVx );
    if( FAILED( hr ) )
        return hr;

    for( DWORD i = 0; i < dwNumVx; ++ i )
    {
        if( fabs( pVx->vPos.x ) < 0.01 )
        {
            if( pVx->vPos.y > 0 )
            {
                pVx->fTex[ 0 ] = 0.0f;
                pVx->fTex[ 1 ] = 0.0f;
            } else
            if( pVx->vPos.y < 0.0f )
            {
                pVx->fTex[ 0 ] = 1.0f * FLOOR_TILECOUNT;
                pVx->fTex[ 1 ] = 1.0f * FLOOR_TILECOUNT;
            } else
            {
                pVx->fTex[ 0 ] = 0.5f * FLOOR_TILECOUNT;
                pVx->fTex[ 1 ] = 0.5f * FLOOR_TILECOUNT;
            }
        } else
        if( pVx->vPos.x > 0.0f )
        {
            pVx->fTex[ 0 ] = 1.0f * FLOOR_TILECOUNT;
            pVx->fTex[ 1 ] = 0.0f;
        } else
        {
            pVx->fTex[ 0 ] = 0.0f;
            pVx->fTex[ 1 ] = 1.0f * FLOOR_TILECOUNT;
        }

        ++ pVx;
    }

    g_pMeshFloor->UnlockVertexBuffer();

    // reset the timer
    g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();;

    // Adjust the dialog parameters.
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, 0/*pBackBufferSurfaceDesc->Height-270*/ );
    g_SampleUI.SetSize( 170, 220 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
#ifdef OLD_TINY
    vector< CTiny* >::iterator itCur, itEnd = g_v_pCharacters.end();
    for( itCur = g_v_pCharacters.begin(); itCur != itEnd; ++ itCur )
        ( * itCur )->Animate( fTime - g_fLastAnimTime );
#endif

	g_pWorld->Update();
	g_pWorld->Animate( fTime - g_fLastAnimTime );

    g_fLastAnimTime = fTime;

    // Update the camera's position based on user input
    g_Camera.FrameMove( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    HRESULT hr;

    pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       D3DCOLOR_ARGB( 0, 0x3F, 0xAF, 0xFF ), 1.0f, 0L );

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // set up the camera
        D3DXMATRIXA16 mx, mxView, mxProj;
        D3DXVECTOR3 vEye;
        D3DXVECTOR3 vLightDir;

        // are we following a tiny, or an independent arcball camera?
        if( g_dwFollow == 0xffffffff )
        {
            // Light direction is same as camera front (reversed)
            vLightDir = -(*g_Camera.GetWorldAhead());

            // set static transforms
            mxView = *g_Camera.GetViewMatrix();
            mxProj = *g_Camera.GetProjMatrix();
            V( pd3dDevice->SetTransform( D3DTS_VIEW, & mxView ) );
            V( pd3dDevice->SetTransform( D3DTS_PROJECTION, & mxProj ) );
            vEye = *g_Camera.GetEyePt();
        }
        else
        {
            // set follow transforms
            CTiny * pChar = g_v_pCharacters[ g_dwFollow ];

            D3DXVECTOR3 vCharPos = pChar->GetOwner()->GetBody().GetPos();
            D3DXVECTOR3 vCharFacing = pChar->GetOwner()->GetBody().GetDir();
            vEye = D3DXVECTOR3  ( vCharPos.x, 0.25f, vCharPos.z );
            D3DXVECTOR3 vAt     ( vCharPos.x, 0.0125f, vCharPos.z ),
                        vUp     ( 0.0f, 1.0f, 0.0f );
            vCharFacing.x *= .25; vCharFacing.y = 0.f; vCharFacing.z *= .25;
            vEye -= vCharFacing;
            vAt += vCharFacing;

            D3DXMatrixLookAtLH( & mxView, & vEye, & vAt, & vUp );
            V( pd3dDevice->SetTransform( D3DTS_VIEW, & mxView ) );

            const D3DSURFACE_DESC *pBackBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
            float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
            D3DXMatrixPerspectiveFovLH( &mxProj, D3DX_PI / 3, fAspectRatio, 0.1f, 100.0f );
            V( pd3dDevice->SetTransform( D3DTS_PROJECTION, & mxProj ) );

            // Set the light direction and normalize
            D3DXVec3Subtract( &vLightDir, &vEye, &vAt );
            D3DXVec3Normalize( &vLightDir, &vLightDir );
        }

        // set view-proj
        D3DXMatrixMultiply( & mx, & mxView, & mxProj );
        g_pEffect->SetMatrix( "g_mViewProj", & mx );
        ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
        if( pMAEffect )
        {
            pMAEffect->SetMatrix( "g_mViewProj", & mx );
        }

        // Set the light direction so that the
        // visible side is lit.
        D3DXVECTOR4 v( vLightDir.x, vLightDir.y, vLightDir.z, 1.0f );
        g_pEffect->SetVector( "lhtDir", &v );
        if( pMAEffect )
            pMAEffect->SetVector( "lhtDir", &v );

        SAFE_RELEASE( pMAEffect );

        // set the fixed function shader for drawing the floor
        V( pd3dDevice->SetFVF( g_pMeshFloor->GetFVF() ) );

        // Draw the floor
        V( g_pEffect->SetTexture( "g_txScene", g_pTxFloor ) );
        V( g_pEffect->SetMatrix( "g_mWorld", &g_mxFloor ) );
        UINT cPasses;
        V( g_pEffect->Begin( &cPasses, 0 ) );
        for( UINT p = 0; p < cPasses; ++p )
        {
            V( g_pEffect->BeginPass( p ) );
            V( g_pMeshFloor->DrawSubset( 0 ) );
            V( g_pEffect->EndPass() );
        }
        V( g_pEffect->End() );

		g_pWorld->AdvanceTimeAndDraw( pd3dDevice, &mx, fElapsedTime, &vEye );

#ifdef OLD_TINY
        // draw each tiny
        vector< CTiny* >::iterator itCur, itEnd = g_v_pCharacters.end();
        for( itCur = g_v_pCharacters.begin(); itCur != itEnd; ++ itCur )
        {
            // set the time to update the hierarchy
            ( * itCur )->AdvanceTime( fElapsedTime, & vEye );
            // draw the mesh
            ( * itCur )->Draw();
        }
#endif


        //
        // Output text information
        //
        RenderText();

        V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SampleUI.OnRender( fElapsedTime ) );

        pd3dDevice->EndScene();
    }
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	int y = 5;
	LPRECT winRect = new RECT;
	GetWindowRect(DXUTGetHWND(), winRect);
	long winMidHor = (winRect->left + winRect->right) / 2 - winRect->left-80;
	long winMidVer = (winRect->bottom + winRect->top) / 2 - winRect->top;
	long winWidth = winRect->right - winRect->left;
	long winHeight = winRect->top - winRect->bottom;
	int bBW = (int)(winWidth*0.09);	//button width
	int bBH = (int)(winHeight*(-0.046));	//button height
	int rightSide = 170;

    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
	
    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    //txtHelper.DrawTextLine( DXUTGetFrameStats() );
    //txtHelper.DrawTextLine( DXUTGetDeviceStats() );

	//Print on Cat&Mouse win
	txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	txtHelper.SetInsertionPos(winMidHor - 125, y);
	txtHelper.DrawFormattedTextLine(L"CAT/Train: %d", g_trainCatWin);
	
	txtHelper.SetInsertionPos(winMidHor - 10, y);
	txtHelper.DrawFormattedTextLine(L"MOUSE/Train: %d", g_trainMouseWin);

	txtHelper.SetInsertionPos(winMidHor+120, y);
	float winningRateTraining = 0;
	if (g_trainCatWin + g_trainMouseWin > 0)
		winningRateTraining = static_cast<float>(g_trainMouseWin) / (g_trainCatWin + g_trainMouseWin)*100;
	txtHelper.DrawFormattedTextLine(L"winning rate/Train: %f", winningRateTraining);
	//Print on Cat&Mouse win
	txtHelper.SetInsertionPos(winMidHor - 125, y+=21);
	txtHelper.DrawFormattedTextLine(L"CAT: %d", g_catWin);
	txtHelper.SetInsertionPos(winMidHor - 10, y);
	txtHelper.DrawFormattedTextLine(L"MOUSE: %d", g_mouseWin);

	txtHelper.SetInsertionPos(winMidHor + 120, y);
	float winningRatePlaying = 0;
	if (g_catWin + g_mouseWin > 0)
		winningRatePlaying = static_cast<float>(g_mouseWin) / (g_catWin + g_mouseWin) * 100;
	txtHelper.DrawFormattedTextLine(L"winning rate: %f", winningRatePlaying);
	//y += 35;int bBH = (int)(winHeight*(-0.092));	//button height
	//print current iteration
	g_cureIteration = g_catWin + g_mouseWin;

	txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	txtHelper.SetInsertionPos(winRect->right - winRect->left - (int)(2.05*bBW), y += (int)(winHeight*(-0.086)*1.75));

	txtHelper.DrawFormattedTextLine(L"Epochs:%d", g_cureIteration);

	//print train time
	txtHelper.SetInsertionPos(winRect->right - winRect->left - (int)(2.05*bBW), y += (int)(bBH*3.5));
	txtHelper.DrawFormattedTextLine(L"LOOP:%d", g_trainloop);

	//print method
	txtHelper.SetInsertionPos(winRect->right - winRect->left - (int)(2.05*bBW), y += (int)(bBH * 2));
	if (g_useQR)
		txtHelper.DrawFormattedTextLine(L"Current:Q-Learning");
	else
		txtHelper.DrawFormattedTextLine(L"Current:SARSA");

	int lefty = (int)(winHeight*(-0.29)*1.75);
	
	//print greedy/smart and continuous status
	txtHelper.SetInsertionPos(20, lefty);
	if (g_useSmartMouse)
		txtHelper.DrawFormattedTextLine(L"Use Smart Mouse");
	else
		txtHelper.DrawFormattedTextLine(L"Use Greedy Mouse");

	txtHelper.SetInsertionPos(20, lefty += (int)(bBH * 2));
	
	if (g_playContinuous)
		txtHelper.DrawFormattedTextLine(L"Continuous: On");
	else
		txtHelper.DrawFormattedTextLine(L"Continuous: Off");

	//print punish and reward
	txtHelper.SetInsertionPos(5, 5);
	txtHelper.DrawFormattedTextLine(L"Punish:%f", g_punish);
	txtHelper.SetInsertionPos(5, 20);
	txtHelper.DrawFormattedTextLine(L"Reward:%f", g_reward);

	
	//print speed info
	txtHelper.SetInsertionPos(5, 40);
	switch (g_RLspeed)
	{
    case RLSpeed::Slow:
		txtHelper.DrawFormattedTextLine(L"Speed Level:	Slow");
		break;
    case RLSpeed::Medium:
		txtHelper.DrawFormattedTextLine(L"Speed Level:	Medium");
		break;
    case RLSpeed::Fast:
		txtHelper.DrawFormattedTextLine(L"Speed Level:	Fast");
		break;
    case RLSpeed::Turbo:
		txtHelper.DrawFormattedTextLine(L"Speed Level:	Turbo");
        break;
    case RLSpeed::SuperSlow:
	default:
		txtHelper.DrawFormattedTextLine(L"Speed Level:	Super slow");
		break;
	}

	//print training status info
	//Choose the one you like, or both
	//Method NO.1
    txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	txtHelper.SetInsertionPos(5, 60);
	switch (g_trainingStatus)
	{
    case 0:
        txtHelper.DrawFormattedTextLine(L"Training Status:");
        txtHelper.SetInsertionPos(105, 60);
        txtHelper.DrawFormattedTextLine(L"Waiting.");
		break;
    case 1:
        txtHelper.DrawFormattedTextLine(L"Training Status:");
        txtHelper.SetInsertionPos(105, 60);
        txtHelper.DrawFormattedTextLine(L"Training...");
		break;
    case 2:
        txtHelper.DrawFormattedTextLine(L"Training Status:	");
        txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
        txtHelper.SetInsertionPos(105, 60);
        txtHelper.DrawFormattedTextLine(L"Training complete!");
		break;	
    case 3:
        txtHelper.DrawFormattedTextLine(L"Training Status:	");
        txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.6f, 1.0f));
        txtHelper.SetInsertionPos(105, 60);
        txtHelper.DrawFormattedTextLine(L"Playing game...");
        break;
    default:
        txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
        txtHelper.DrawFormattedTextLine(L"Training Status:	");
        txtHelper.SetForegroundColor(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
        txtHelper.SetInsertionPos(105, 60);
        txtHelper.DrawFormattedTextLine(L"I don't know?");
		break;
	}

	//Method NO.2
	//txtHelper.SetForegroundColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	//txtHelper.SetInsertionPos(winMidHor - 60, winMidVer);
	//if (g_trainingStatus)
	//	txtHelper.DrawFormattedTextLine(L"Training Complete!!");

	// Print out states
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
	dbCompositionList list;
	g_database.ComposeList( list, OBJECT_Ignore_Type );
	int starttext = 80;
	int count = 0;
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine )
		{
			char* name = (*i)->GetName();
			char* statename = pStateMachine->GetCurrentStateNameString();
			char* substatename = pStateMachine->GetCurrentSubstateNameString();
			TCHAR* unicode_name = new TCHAR[strlen(name)+1];
			TCHAR* unicode_statename = new TCHAR[strlen(statename)+1];
			TCHAR* unicode_substatename = new TCHAR[strlen(substatename)+1];
			mbstowcs(unicode_name, name, strlen(name)+1);
			mbstowcs(unicode_statename, statename, strlen(statename)+1);
			mbstowcs(unicode_substatename, substatename, strlen(substatename)+1);
			if( substatename[0] != 0 )
			{
				txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 5, starttext-1+(12*count) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s, %s", unicode_name, unicode_statename, unicode_substatename );
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 4, starttext+(12*count++) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s, %s", unicode_name, unicode_statename, unicode_substatename );
			}
			else
			{
				txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 4, starttext-1+(12*count) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s", unicode_name, unicode_statename );
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 5, starttext+(12*count++) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s", unicode_name, unicode_statename );
			}
			delete unicode_name;
			delete unicode_statename;
			delete unicode_substatename;
		}
	}
	

    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.SetInsertionPos( 5, 70 );
	
    // We can only display either the behavior text or help text,
    // with the help text having priority.
    if( g_bShowHelp )
    {
        // output data for T[m_dwFollow]
        if( g_dwFollow != -1 )
        {
            //txtHelper.DrawTextLine( L"Press F1 to hide animation info\n"
            //                        L"Quit: Esc" );

            if( g_v_pCharacters[ g_dwFollow]->IsUserControl() )
            {
                txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width - 150, 150 );
                txtHelper.DrawTextLine( L"       Tiny control:\n"
                                        L"Move forward\n"
                                        L"Run forward\n"
                                        L"Turn\n" );
                txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width - 55, 150 );
                txtHelper.DrawTextLine( L"\n"
                                        L"W\n"
                                        L"Shift W\n"
                                        L"A,D\n" );
            }

            int i;
            CTiny * pChar = g_v_pCharacters[ g_dwFollow ];
            vector < String > v_sReport;
            pChar->Report( v_sReport );
            txtHelper.SetInsertionPos( 5, pd3dsdBackBuffer->Height - 115 );
            for( i = 0; i < 6; ++i )
            {
                txtHelper.DrawTextLine(v_sReport[i].c_str() );
            }
            txtHelper.DrawTextLine( v_sReport[16].c_str() );

            txtHelper.SetInsertionPos( 210, pd3dsdBackBuffer->Height - 85 );
            for( i = 6; i < 11; ++i )
            {
                txtHelper.DrawTextLine( v_sReport[i].c_str() );
            }

            txtHelper.SetInsertionPos( 370, pd3dsdBackBuffer->Height - 85 );
            for( i = 11; i < 16; ++i )
            {
                txtHelper.DrawTextLine( v_sReport[i].c_str() );
            }
        } else
		{
            //txtHelper.DrawTextLine( L"\n"
            //                        L"Quit: Esc" );
		}
    } else
    {
        //if( g_dwFollow != 0xffffffff )
        //    txtHelper.DrawTextLine( L"Press F1 to display animation info\n"
        //                            L"Quit: Esc" );
        //else
        //    txtHelper.DrawTextLine( L"\n"
        //                            L"Quit: Esc" );
    }

    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;


	if( uMsg == WM_LBUTTONDOWN )
	{
		IDirect3D9 *d3d = DXUTGetD3D9Object();
		IDirect3DDevice9 *dev = DXUTGetD3D9Device();
		if( d3d && dev )
		{
			g_click2D.x = (float)LOWORD(lParam);
			g_click2D.y = (float)HIWORD(lParam);

			D3DVIEWPORT9 vp;
			dev->GetViewport( &vp );

			D3DXMATRIX world;
			D3DXMatrixIdentity( &world );

			D3DXMATRIX view;
			dev->GetTransform( D3DTS_VIEW, &view );

			D3DXMATRIX projection;
			dev->GetTransform( D3DTS_PROJECTION, &projection );

			D3DXVECTOR3 nearClick3D;
			D3DXVECTOR3 farClick3D;

			g_click2D.z = vp.MinZ;
			D3DXVec3Unproject( &nearClick3D, &g_click2D, &vp, &projection, &view, &world );

			g_click2D.z = vp.MaxZ;
			D3DXVec3Unproject( &farClick3D, &g_click2D, &vp, &projection, &view, &world );

			D3DXVECTOR3 ray = farClick3D - nearClick3D;
			if( ray.y != 0.0f )
			{
				g_click3D = nearClick3D + (-nearClick3D.y / ray.y) * ray;
			}

			g_click3D.y = 0.0f;
            // g_database.SendMsgFromSystem( MSG_SetGoal, MSG_Data(g_click3D)); // Project 2 and 3 version
            g_database.SendMsgFromSystem( MSG_MouseClick, MSG_Data(g_click3D)); // Project 1 version
		}
		return 0;
	}
	if (uMsg == WM_SIZE)
	{
		//window resize redraw all the buttoms
		RedrawButtons();
	}

    // Pass messages to camera class for camera movement if the
    // global camera if active
    if( -1 == g_dwFollow )
        g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1:
				g_bShowHelp = !g_bShowHelp;
				break;

            case VK_F10:
				g_database.SendMsgFromSystem( MSG_Reset );
				break;

        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
        case IDC_ADDTINY:	

		case IDC_NEXTMAP:
			g_terrain.NextMap();
			break;


case IDC_PUNISH:
	if (g_punish == 0.0f)		{ g_punish = 10.f; }
	else if (g_punish == 10.f)	{ g_punish = 12.f; }
	else if (g_punish == 12.f)	{ g_punish = 15.f; }
    else if (g_punish == 15.f)	{ g_punish = 20.f; }
    else if (g_punish == 20.f)	{ g_punish = 50.f; }
    else if (g_punish == 50.f)	{ g_punish = 100.f;}
	else						{ g_punish = 0.0f; }
	g_database.SendMsgFromSystem(MSG_SetPunish, MSG_Data(g_punish));
	break;

case IDC_REWARD:
	if (g_reward == 0.0f)		{ g_reward = 10.f; }
	else if (g_reward == 10.f)	{ g_reward = 12.f; }
	else if (g_reward == 12.f)	{ g_reward = 15.f; }
    else if (g_reward == 15.f)	{ g_reward = 20.f; }
    else if (g_reward == 20.f)	{ g_reward = 50.f; }
    else if (g_reward == 50.f)	{ g_reward = 100.f;}
	else						{ g_reward = 0.0f; }
	g_database.SendMsgFromSystem(MSG_SetReward, MSG_Data(g_reward));
	break;
case IDC_LOOP_1000:
	g_trainloop = 1000;
	g_database.SendMsgFromSystem(MSG_SetTrainLoop, MSG_Data(g_trainloop));
	break;
case IDC_LOOP_5000:
	g_trainloop = 5000;
	g_database.SendMsgFromSystem(MSG_SetTrainLoop, MSG_Data(g_trainloop));
	break;
case IDC_LOOP_10000:
	g_trainloop = 10000;
	g_database.SendMsgFromSystem(MSG_SetTrainLoop, MSG_Data(g_trainloop));
	break;
case IDC_LOOP_50000:
	g_trainloop = 50000;
	g_database.SendMsgFromSystem(MSG_SetTrainLoop, MSG_Data(g_trainloop));
	break;
case IDC_METHOD_QL:
	g_useQR = true;
	g_database.SendMsgFromSystem(MSG_SetMethod_UseQL, MSG_Data(g_useQR));
	break;
case IDC_METHOD_SARSA:
	g_useQR = false;
	g_database.SendMsgFromSystem(MSG_SetMethod_UseQL, MSG_Data(g_useQR));
	break;
case IDC_RESET_RL:
	g_database.SendMsgFromSystem(MSG_ResetLearner);
	break;
case IDC_CLEAR_SCORE:
	g_catWin = 0;
	g_mouseWin = 0;
	g_trainCatWin = 0;
	g_trainMouseWin = 0;
	g_database.SendMsgFromSystem(MSG_ClearScores);
	break;
case IDC_START_TRAINING:
    g_database.SendMsgFromSystem(MSG_StartLearning, MSG_Data(g_useQR));
	break;
case IDC_START_PLAYING:
    g_database.SendMsgFromSystem(MSG_StartPlaying, MSG_Data(g_useQR));
	break;

case IDC_SPEED_SUPERSLOW:
    g_RLspeed = RLSpeed::SuperSlow;
    g_database.SendMsgFromSystem(MSG_SetRLSpeed, MSG_Data(g_RLspeed));
	break;
case IDC_SPEED_SLOW:
    g_RLspeed = RLSpeed::Slow;
    g_database.SendMsgFromSystem(MSG_SetRLSpeed, MSG_Data(g_RLspeed));
	break;
case IDC_SPEED_MEDIUM:
    g_RLspeed = RLSpeed::Medium;
    g_database.SendMsgFromSystem(MSG_SetRLSpeed, MSG_Data(g_RLspeed));
	break;
case IDC_SPEED_FAST:
    g_RLspeed = RLSpeed::Fast;
    g_database.SendMsgFromSystem(MSG_SetRLSpeed, MSG_Data(g_RLspeed));
	break;
case IDC_SPEED_TURBO:
    g_RLspeed = RLSpeed::Turbo;
    g_database.SendMsgFromSystem(MSG_SetRLSpeed, MSG_Data(g_RLspeed));
	break;

case IDC_TOOGLE_CONTINUOUS:
	if (g_playContinuous) g_playContinuous = false;
	else g_playContinuous = true;
	g_database.SendMsgFromSystem(MSG_SetPlayContinuous, MSG_Data(g_playContinuous));
	break;

case IDC_TOOGLE_SMART_MOUSE:
	g_useSmartMouse = true;
	g_database.SendMsgFromSystem(MSG_UseSmartMouse, MSG_Data(g_useSmartMouse));
	break;

case IDC_TOOGLE_GREEDY_MOUSE:
	g_useSmartMouse = false;
	g_database.SendMsgFromSystem(MSG_UseSmartMouse, MSG_Data(g_useSmartMouse));
	break;

    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();
    ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
    if( pMAEffect )
    {
        pMAEffect->OnLostDevice();
        pMAEffect->Release();
    }

    SAFE_RELEASE( g_pTextSprite );
    SAFE_RELEASE( g_pMeshFloor );

    vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
    for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
    {
        ( *itCurCP )->InvalidateDeviceObjects();
    }

    CMultiAnimAllocateHierarchy AH;
    AH.SetMA( & g_MultiAnim );
    g_MultiAnim.Cleanup( & AH );
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE(g_pEffect);
    SAFE_RELEASE(g_pFont);

    SAFE_RELEASE( g_pTxFloor );
    SAFE_RELEASE( g_pMeshFloor );

	//delete g_pWorld;
    g_pWorld->InvalidateDeviceObjects();
}

void  RedrawButtons()
{
	LPRECT winRect = new RECT;
	GetWindowRect(DXUTGetHWND(), winRect);

	long winMidHor = (winRect->left + winRect->right) / 2 - winRect->left;
	long winMidVer = (winRect->bottom + winRect->top) / 2 - winRect->top;
	long winWidth = winRect->right - winRect->left;
	long winHeight = winRect->top - winRect->bottom;

	g_SampleUI.RemoveAllControls();
	g_SampleUI.Init(&g_DialogResourceManager);
	
	int gapVer = (int)(-winHeight*0.1);
	int iY = (int)(-winHeight*0.02);

	int sBHorOffset1 = (int)(winWidth / 2 * 0.25);
	int sBHorOffset2 = (int)(winWidth / 2 * 0.435);
	g_SampleUI.SetCallback(OnGUIEvent); 
	int rightSide = 170;
	
	int bBW = (int)(winWidth*0.18);	//button width
	int bBH = (int)(winHeight*(-0.092));	//button height
	g_SampleUI.AddButton(IDC_NEXTMAP, L"Next Map", rightSide - (int)(1.02*bBW), iY, bBW, bBH);

	bBW = (int)(winWidth*0.09);	//button width
	bBH = (int)(winHeight*(-0.046));	//button height
	
	g_SampleUI.AddButton(IDC_PUNISH, L"Punish", rightSide - (int)(2.05*bBW), iY += gapVer, bBW, bBH);
	g_SampleUI.AddButton(IDC_REWARD, L"Reward", rightSide - bBW, iY, bBW, bBH);

	g_SampleUI.AddButton(IDC_LOOP_1000, L"1000", rightSide - (int)(2.05*bBW), iY += gapVer, bBW, bBH);
	g_SampleUI.AddButton(IDC_LOOP_5000, L"5000", rightSide - bBW, iY, bBW, bBH);

	g_SampleUI.AddButton(IDC_LOOP_10000, L"10000", rightSide - (int)(2.05*bBW), iY += (int)(gapVer / 2), bBW, bBH);
	g_SampleUI.AddButton(IDC_LOOP_50000, L"50000", rightSide - bBW, iY, bBW, bBH);

	g_SampleUI.AddButton(IDC_METHOD_QL, L"Q-Learning", rightSide - (int)(2.05*bBW), iY += gapVer, bBW, bBH);
	g_SampleUI.AddButton(IDC_METHOD_SARSA, L"SARSA", rightSide - bBW, iY, bBW, bBH);

	bBW = (int)(winWidth*0.18);	        // button width
	bBH = (int)(winHeight*(-0.092));	// button height

	g_SampleUI.AddButton(IDC_START_TRAINING, L"Start/stop training", rightSide - (int)(1.02*bBW), iY += gapVer, bBW, bBH);

	g_SampleUI.AddButton(IDC_RESET_RL, L"Reset Learner", rightSide - (int)(1.02*bBW), iY += (int)(gapVer * 1.4), bBW, bBH/2);
	g_SampleUI.AddButton(IDC_CLEAR_SCORE, L"Clear Scores", rightSide - (int)(1.02*bBW), iY += gapVer/2, bBW, bBH/2);

	iY = (int)(-winHeight*0.4);

	bBW = (int)(winWidth*0.09);	//button width
	bBH = (int)(winHeight*(-0.046));	//button height
	int leftSide = 206-winWidth;		//magic number

	g_SampleUI.AddButton(IDC_TOOGLE_SMART_MOUSE, L"Smart", leftSide, iY += gapVer / 2, bBW, bBH);
	g_SampleUI.AddButton(IDC_TOOGLE_GREEDY_MOUSE, L"Greedy", leftSide + (int)(1.02*bBW), iY, bBW, bBH);
	
	g_SampleUI.AddButton(IDC_TOOGLE_CONTINUOUS, L"Play Continuous", leftSide, iY += gapVer, bBW*2, bBH);
	
	g_SampleUI.AddButton(IDC_START_PLAYING, L"Start/stop playing", leftSide, iY += gapVer, bBW*2, bBH*2);
	
	
	int sBVerOffset = (int)(winHeight / 2 * 0.78);
	int sBHorOffset = (int)(winWidth / 2 * 0.25);
	int sBW         = (int)(winWidth*0.1);			// speed button width
	int sBH         = (int)(winHeight*(-0.03));	    // speed button height
	
	int magicNumber = 130;
    g_SampleUI.AddButton(IDC_SPEED_FAST,        L"Fast",    -(winMidHor - sBHorOffset - magicNumber),   winMidVer - sBVerOffset, sBW, sBH);
    g_SampleUI.AddButton(IDC_SPEED_MEDIUM,      L"Medium",  -(winMidHor - magicNumber),                 winMidVer - sBVerOffset, sBW, sBH);
	g_SampleUI.AddButton(IDC_SPEED_SLOW,        L"Slow",    -(winMidHor + sBHorOffset - magicNumber),   winMidVer - sBVerOffset, sBW, sBH);

	sBHorOffset = (int)(winWidth / 2 * 0.50);
    g_SampleUI.AddButton(IDC_SPEED_TURBO,       L"Turbo",       -(winMidHor - sBHorOffset - magicNumber), winMidVer - sBVerOffset, sBW, sBH);
    g_SampleUI.AddButton(IDC_SPEED_SUPERSLOW,   L"Super slow",  -(winMidHor + sBHorOffset - magicNumber), winMidVer - sBVerOffset, sBW, sBH);
	
}