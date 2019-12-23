#include "stdafx.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

double GetSecondsFromStart()
{
	clock_t t = clock();
	return (double)t / CLOCKS_PER_SEC;
}

LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LPDIRECT3DVERTEXBUFFER9 indicatorV_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 indicatorI_buffer = NULL;
LPDIRECT3DVERTEXBUFFER9 floorV_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 floorI_buffer = NULL;
LPDIRECT3DVERTEXBUFFER9 healthBarV_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 healthBarI_buffer = NULL;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;   
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;   

void initD3D(HWND hWnd);
void render_frame(GameData& d);
void cleanD3D(void);
void init_graphics(void);

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

LRESULT CALLBACK WindowProc(HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	srand(time(0));
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "DodgeGame";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		"DodgeGame", 
		"Dodge Game",
		WS_EX_TOPMOST | WS_POPUP,
		0, 
		0, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT,
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	ShowWindow(hWnd, nCmdShow);

	initD3D(hWnd);

	MSG msg;
	GameData d;
	Player p1;
	d.m_pPlayer = &p1;
	time_t start = time(0);
	float enemySpeed = 0.1f;
	float spawnDelay = 1.0f;
	double t = GetSecondsFromStart();

	bool downPressed = false;
	bool upPressed = false;
	bool rightPressed = false;
	bool leftPressed = false;
	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		double t2 = GetSecondsFromStart();
		double dt = t2 - t;

		Enemy* pProjEnemy = d.m_pProjEnemyRoot;
		for (int i = 0; i < d.m_projCount; i++)
		{
			pProjEnemy->Move();
			Enemy* temp = pProjEnemy->next;
			if (pProjEnemy->CollideWithPlayer())
			{
				d.RemoveEnemy(pProjEnemy);
				pProjEnemy = temp;
				continue;
			}
			if (pProjEnemy->m_position.x > 25.0f)
				d.RemoveEnemy(pProjEnemy);
			pProjEnemy = temp;
		}



		if (GetAsyncKeyState('W'))
		{
			if (!upPressed)
			{
				d.m_pPlayer->m_direction.z += -1;
				upPressed = true;
			}
		}
		else
		{
			if (upPressed)
			{
				d.m_pPlayer->m_direction.z += 1;
				upPressed = false;
			}
		}
		if (GetAsyncKeyState('S'))
		{
			if (!downPressed)
			{
				d.m_pPlayer->m_direction.z += 1;
				downPressed = true;
			}
		}
		else
		{
			if (downPressed)
			{
				d.m_pPlayer->m_direction.z += -1;
				downPressed = false;
			}
		}
		if (GetAsyncKeyState('D'))
		{
			if (!leftPressed)
			{
				d.m_pPlayer->m_direction.x += -1;
				leftPressed = true;
			}
		}
		else
		{
			if (leftPressed)
			{
				d.m_pPlayer->m_direction.x += 1;
				leftPressed = false;
			}
		}
		if (GetAsyncKeyState('A'))
		{
			if (!rightPressed)
			{
				d.m_pPlayer->m_direction.x += 1;
				rightPressed = true;
			}
		}
		else
		{
			if (rightPressed)
			{
				d.m_pPlayer->m_direction.x += -1;
				rightPressed = false;
			}
		}
		d.m_pPlayer->Move();


		if (t < 25)
		{
			if (enemySpeed < 1.0f)
			{
				enemySpeed +=  dt / 10000.0f;
			}

			if (spawnDelay > 0.01f)
			{
				spawnDelay -= dt / 10000.0f;
			}
		}
		else
		{
			if (enemySpeed < 1.0f)
			{
				enemySpeed += dt / 500.0f;
			}

			if (spawnDelay > 0.01f)
			{
				spawnDelay -= dt / 100.0f;
			}
		}


			if (dt > spawnDelay)
			{
				d.AddEnemy(enemySpeed);
				t = t2;
			}

		render_frame(d);

		if (d.m_pPlayer->m_health <= 0)
			break;
	}

	cleanD3D();

	return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();

	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    
}


// this is the function used to render a single frame
void render_frame(GameData& d)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	d3ddev->SetFVF(CUSTOMFVF);

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 30.0f, 3.0f),
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	d3ddev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
		1.0f,
		100.0f);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX floorMat;
	D3DXMatrixTranslation(&floorMat, 0.0f, -1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &floorMat);
	d3ddev->SetIndices(floorI_buffer);
	d3ddev->SetStreamSource(0, floorV_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	D3DXMATRIX healthBarMat;
	D3DXMATRIX healthBarScale;
	D3DXMatrixTranslation(&healthBarMat, (10.0f - d.m_pPlayer->m_health * 0.1f), -1.0f, -12.0f);
	D3DXMatrixScaling(&healthBarScale, (float)d.m_pPlayer->m_health / 100.0f, 1.0f, 1.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &(healthBarScale *healthBarMat));
	d3ddev->SetIndices(healthBarI_buffer);
	d3ddev->SetStreamSource(0, healthBarV_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);


	float posX = -16.5f + (rand() % 3300)/100.0f;
	float posZ = -8.5f + (rand() % 1700)/100.0f;
	D3DXMATRIX indicatorMat;
	D3DXMatrixTranslation(&indicatorMat, posX, 0.1f, posZ);
	d3ddev->SetTransform(D3DTS_WORLD, &indicatorMat);
	d3ddev->SetIndices(indicatorI_buffer);
	d3ddev->SetStreamSource(0, indicatorV_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 8);

	D3DXMATRIX playerMat;
	D3DXMatrixTranslation(&playerMat, d.m_pPlayer->m_position.x, 0.0f, d.m_pPlayer->m_position.z);
	d3ddev->SetTransform(D3DTS_WORLD, &playerMat);
	d3ddev->SetIndices(i_buffer);
	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12); 

	Enemy* pProjEnemy = d.m_pProjEnemyRoot;
	for (int i = 0; i < d.m_projCount; i++)
	{
		D3DXMatrixTranslation(&pProjEnemy->enemyMat, pProjEnemy->m_position.x, 0.0f, pProjEnemy->m_position.z);
		d3ddev->SetTransform(D3DTS_WORLD, &pProjEnemy->enemyMat);
		d3ddev->SetIndices(i_buffer);
		d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		pProjEnemy = pProjEnemy->next;
	}

	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);

}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	indicatorI_buffer->Release();
	indicatorV_buffer->Release();
	healthBarI_buffer->Release();
	healthBarV_buffer->Release();
	floorV_buffer->Release();
	floorI_buffer->Release();
	v_buffer->Release();
	i_buffer->Release();
	d3ddev->Release();
	d3d->Release();
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	struct CUSTOMVERTEX cubeVertexes[] =
	{
		//Cube
		{0.5f, 1.0f, 0.5f, D3DCOLOR_XRGB(255,0,0)},
		{-0.5f, 1.0f, 0.5f, D3DCOLOR_XRGB(0,0,255)},
		{0.5f, -1.0f, 0.5f, D3DCOLOR_XRGB(255,0,0)},
		{-0.5f, -1.0f, 0.5f, D3DCOLOR_XRGB(0,0,255)},
		{0.5f, 1.0f, -0.5f, D3DCOLOR_XRGB(255,0,0)},
		{-0.5f, 1.0f, -0.5f, D3DCOLOR_XRGB(0,0,255)},
		{0.5f, -1.0f, -0.5f, D3DCOLOR_XRGB(255,0,0)},
		{-0.5f, -1.0f, -0.5f, D3DCOLOR_XRGB(0,0,255)}
	};

	struct CUSTOMVERTEX floorVertex[] =
	{
		{20.0f, 0.0f, 10.0f, D3DCOLOR_XRGB(255, 255, 0) },
		{-20.0f, 0.0f, 10.0f, D3DCOLOR_XRGB(255, 255, 0) },
		{20.0f, 0.0f, -10.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{-20.0f, 0.0f, -10.0f, D3DCOLOR_XRGB(0, 255, 0) },
	};

	struct CUSTOMVERTEX healthBarVertex[] =
	{
		{10.0f, 0.0f, 0.5f, D3DCOLOR_XRGB(0,255,0)},
		{-10.0f, 0.0f, 0.5f, D3DCOLOR_XRGB(0,255,0)},
		{10.0f, 0.0f, -0.5f, D3DCOLOR_XRGB(0,255,0)},
		{-10.0f, 0.0f, -0.5f, D3DCOLOR_XRGB(0,255,0)},
	};

	struct CUSTOMVERTEX indicatorVertex[] =
	{
		{2.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)},
		{-2.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)},
		{2.0f, 0.0f, -1.0f, D3DCOLOR_XRGB(255,0,0)},
		{-2.0f, 0.0f, -1.0f, D3DCOLOR_XRGB(255,0,0)},

		{2.2f, 0.0f, 1.2f, D3DCOLOR_XRGB(255,0,0)},
		{-2.2f, 0.0f, 1.2f, D3DCOLOR_XRGB(255,0,0)},
		{2.2f, 0.0f, -1.2f, D3DCOLOR_XRGB(255,0,0)},
		{-2.2f, 0.0f, -1.2f, D3DCOLOR_XRGB(255,0,0)},
	};

	d3ddev->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);
	d3ddev->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&floorV_buffer,
		NULL);
	d3ddev->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&healthBarV_buffer,
		NULL);
	d3ddev->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&indicatorV_buffer,
		NULL);

	VOID* pVoid;

	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, cubeVertexes, sizeof(cubeVertexes));
	v_buffer->Unlock();

	floorV_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, floorVertex, sizeof(floorVertex));
	floorV_buffer->Unlock();

	healthBarV_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, healthBarVertex, sizeof(healthBarVertex));
	healthBarV_buffer->Unlock();

	indicatorV_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indicatorVertex, sizeof(indicatorVertex));
	indicatorV_buffer->Unlock();

	short cubeIndices[] =
	{
		0, 1, 2,
		1, 2, 3,
		0, 1, 4,
		1, 4, 5,
		0, 2, 4,
		2, 4, 6,
		1, 3, 5,
		3, 5, 7,
		2, 3, 6,
		3, 6, 7,
		4, 5, 6,
		5, 6, 7
	};

	short floorIndices[] =
	{
		0, 1, 2,
		1, 2, 3
	}; 

	short healthBarIndices[] =
	{
		0, 1, 2,
		1, 2, 3
	};

	short indicatorIndices[] =
	{
		0, 1, 4,
		5, 4, 1,
		0, 4, 6,
		0, 6, 2,
		2, 6, 7,
		2, 7, 3,
		1, 3, 5,
		3, 7, 5
	};

	d3ddev->CreateIndexBuffer(36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);
	d3ddev->CreateIndexBuffer(6 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&floorI_buffer,
		NULL);
	d3ddev->CreateIndexBuffer(6 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&healthBarI_buffer,
		NULL);
	d3ddev->CreateIndexBuffer(24 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&indicatorI_buffer,
		NULL);

	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, cubeIndices, sizeof(cubeIndices));
	i_buffer->Unlock(); 

	floorI_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, floorIndices, sizeof(floorIndices));
	floorI_buffer->Unlock();

	healthBarI_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, healthBarIndices, sizeof(healthBarIndices));
	healthBarI_buffer->Unlock();

	indicatorI_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indicatorIndices, sizeof(indicatorIndices));
	indicatorI_buffer->Unlock();
}