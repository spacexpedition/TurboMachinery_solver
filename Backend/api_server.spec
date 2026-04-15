# -*- mode: python ; coding: utf-8 -*-
# PyInstaller spec for Turbomachinery Solver Backend
# Run from d:\Turbomachines_solver\Backend with:
#   pyinstaller api_server.spec

import os

BACKEND_DIR = os.path.abspath('.')

a = Analysis(
    ['api_server.py'],
    pathex=[BACKEND_DIR],
    binaries=[],
    # Bundle the solver logic, services, templates, and the SQLite DB
    datas=[
        ('solver',    'solver'),
        ('services',  'services'),
        ('templates', 'templates'),
    ],
    hiddenimports=[
        # FastAPI / Starlette internals that PyInstaller misses
        'uvicorn.logging',
        'uvicorn.loops',
        'uvicorn.loops.auto',
        'uvicorn.protocols',
        'uvicorn.protocols.http',
        'uvicorn.protocols.http.auto',
        'uvicorn.protocols.websockets',
        'uvicorn.protocols.websockets.auto',
        'uvicorn.lifespan',
        'uvicorn.lifespan.on',
        'anyio',
        'anyio._backends._asyncio',
        'starlette.routing',
        'starlette.middleware',
        'starlette.middleware.cors',
        # Google Auth
        'google.auth',
        'google.auth.transport.requests',
        'google.oauth2.id_token',
        'cachetools',
        # Pydantic v2 core
        'pydantic',
        'pydantic_core',
        # SymPy (solver)
        'sympy',
        'mpmath',
        # Jinja2 templates
        'jinja2',
        'markupsafe',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)

pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='backend_server',          # output: backend_server.exe
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,                   # KEEP True so errors are visible
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
