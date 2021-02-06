#!/usr/bin/python

from scripts.buildutils import build
import sys
import os

projects = [
    {
        'name': 'Birdy3d',
        'path': 'engine',
        'source': '.',
        'build': 'build',
        'include': [
            '/usr/include',
            'vendor/glad/include',
            'vendor/stb_image',
            'src',
            '/usr/include/freetype2'
        ],
        'format': 'shared',
        'linkerargs': '-lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp',
        'copy': [
            {
                'filetypes': [ '.glsl' ],
                'source': 'src/shaders',
                'dest': 'shaders'
            }
        ]
    },
    {
        'name': 'sandbox',
        'path': 'sandbox',
        'source': 'src',
        'build': 'build',
        'include': [
            '/usr/include',
            '../engine/build/include/src',
            '../engine/vendor/glad/include',
            '../engine/vendor/stb_image',
            'src',
            '/usr/include/freetype2'
        ],
        'format': 'executable',
        'linkerargs': '-Wl,-rpath,$ORIGIN -L../engine/build/bin -lglfw -lassimp -lBirdy3d -lfreetype',
        'copy': [
            {
                'filetypes': [ '.so' ],
                'source': '../engine/build/bin',
                'dest': 'bin'
            },
            {
                'filetypes': [ '.glsl' ],
                'source': '../engine/build/shaders',
                'dest': 'shaders'
            },
            {
                'filetypes': [ '' ],
                'source': 'ressources',
                'dest': 'ressources'
            }
        ]
    }
]

# TODO: run

if __name__ == '__main__':
    projectsroot = os.path.abspath(os.path.dirname(os.path.abspath(sys.argv[0])) + '/..')
    found = False
    for project in projects:
        if (len(sys.argv) > 1 and (sys.argv[1] == project['name'] or sys.argv[1] == project['path'])) or os.getcwd().endswith(project['source']):
            found = True
            if project['name'] != 'Birdy3d':
                if not build(projects[0]):
                    break
                print()
            build(project)
            break
    if not found:
        print('Project not found')
