#!/usr/bin/env python3

# This file is part of the Funk code distribution.
# Copyright (c) 2021 Arnaud Andrianavalomahefa and Darko Veberic
# 
# This program is free software: you can redistribute it and/or modify  
# it under the terms of the GNU General Public License as published by  
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import os
import numpy as np
import matplotlib.pyplot as plt
from functools import wraps
from warnings import catch_warnings, filterwarnings

from scipy.interpolate import interp1d


def handle_warnings(func):
    """ decorator to output func args/kwargs values upon warnings """
    @wraps(func)
    def wrapper(*args, **kwargs):
        with catch_warnings(record=True) as ws:
            filterwarnings('always')
            ret = func(*args, **kwargs)
            if len(ws):
                func_sig = signature(func)
                bound    = func_sig.bind(*args, **kwargs)
                bound.apply_defaults()  # kwargs become absorbed into args?
                params_str = '  ' + (
                    '\n  '.join('{} = {}'.format(*s) for s in bound.arguments.items())
                )
                # w = ws[-1] # last warning
                for w in ws:
                    print('[{}] {}'.format(w.category.__name__, w.message))
                print('function {} has been called with args: \n{}\n'.format(func.__name__, params_str))
        return ret
    return wrapper


plt.rcParams.update({
    # tex document \columnwidth = 246pt and \textwidth = 510pt
    # save with plt.tight_layout(pad=0)
    'figure.figsize'      : (0.7*510/72, 0.5*510/72),
    'savefig.format'      : 'pdf',
    'text.latex.preamble' : r'\usepackage{newpxtext}\usepackage{newpxmath}\usepackage{upgreek}\usepackage{amsmath}\usepackage{amssymb}',
    'text.usetex'         : True,
    'font.family'         : 'serif',
    'font.size'           : 9, #10
    'axes.labelsize'      : 9, #10
    'legend.fontsize'     : 9, #10
    'xtick.labelsize'     : 9,
    'ytick.labelsize'     : 9,
    'axes.grid'           : False,
    'grid.linestyle'      : ':',
    'grid.linewidth'      : 0.4,
})

line_props = {
    'cdm'          : { "color" : 'whitesmoke', "zorder" : 1 },
    'xenon10sun'   : { "color" : 'lightgrey', "lw" : 0, "zorder" : 2 },
    'rg'           : { "color" : 'plum', "lw" : 0, "zorder" : 3 },
    'hb'           : { "color" : 'skyblue', "lw" : 0, "zorder" : 4 },
    'thermal'      : { "color" : 'brown', "zorder" : 5 },
    'solarlife'    : { "color" : 'mediumseagreen', "zorder" : 6 },
    'lsw'          : { "color" : 'coral', "zorder" : 7 },
    'cmb'          : { "color" : 'black', "zorder" : 8 },
    'heli1'        : { "color" : 'mediumseagreen', "zorder" : 10 },
    'heli2'        : { "alpha" : 0, "zorder" : 10 },
    'coulomb'      : { "color" : 'slategrey', "zorder" : 11 },
    'halo1'        : { "color" : 'darkslategrey', "zorder" : 12 },
    'halo2'        : { "color" : 'darkslategrey', "zorder" : 13 },
    'xenon10'      : { "color" : 'rosybrown', "lw" : 0, "zorder" : 2 },
    'damic'        : { "color" : 'moccasin', "lw" : 0, "zorder" : 15 },
    'tokyo'        : { "color" : 'midnightblue', "lw" : 0, "zorder" : 16 },
    'tokyo_mev'    : { "color" : 'blue', "lw" : 0, "zorder" : 16 },
    'tokyo_k-band' : { "color" : 'blue', "lw" : 0.5, "zorder" : 16 },
    'shuket'       : { "color" : 'black', "lw" : 0, "zorder" : 17 },
    'funk'         : { "color" : 'crimson', "lw" : 0, "zorder" : 15 },
}


def find_files(path):
    fpaths, fnames = [], []
    for root, dirs, files in os.walk(path):
        for file in files:
            if file[0] != '.':  # ignore hidden files
                fpaths.append(os.path.join(root, file))
                fnames.append(file)
        break  # non-recursive
    return fpaths, fnames


def exclusion_line(data):
    """ data are in log-log scale """
    xs, ys = np.log10(data).T
    xs_new = np.logspace(xs[0], xs[-1], num=500)
    return xs_new, interp1d(xs, ys, kind='linear')


@handle_warnings
def rescale(line):
    """ logscale the interpolated values form exclusion_line() """
    xs, ys = line
    return xs, np.power(10, ys(np.log10(xs)))


def add_line(line, ax=None, **kwargs):
    if ax is None:
        ax = plt.gca()
    ax.plot(*rescale(line), **kwargs)


def add_fill(line1, line2, ax=None, **kwargs):
    if ax is None:
        ax = plt.gca()
    #kwargs.setdefault('facecolor', (0, 0, 0, .5))
    #kwargs.setdefault('edgecolor', (0, 0, 0, 0))
    if isinstance(line2, (int, float)):
        ax.fill_between(*rescale(line1), line2, **kwargs)
    elif line2 == 'ymin':
        ax.fill_between(*rescale(line1), ax.get_ybound()[0], **kwargs)
    elif line2 == 'ymax':
        ax.fill_between(*rescale(line1), ax.get_ybound()[1], **kwargs)
    elif isinstance(line2, (tuple, list)):
        # fill between two curves
        xs1, xs2 = line1[0], line2[0]
        ys1, ys2 = line1[1], line2[1]
        xlow = np.log10(max(xs1[0], xs2[0]))
        xup = np.log10(min(xs1[-1], xs2[-1]))
        xs_common = np.logspace(xlow, xup, num=500)
        ax.fill_between(
            xs_common,
            np.power(10, ys1(np.log10(xs_common))),
            np.power(10, ys2(np.log10(xs_common))),
            **kwargs
        )
    else:
        print('[Warning] add_fill(): check args - skipping plot')


def add_text(txt, coords, ax=None, **kwargs):
    """ coords (tuple) are in data coords (x,y) """
    if ax is None:
        ax = plt.gca()
    kwargs.setdefault('color', 'k')
    #kwargs.setdefault('alpha', 0.55)
    kwargs.setdefault('family', 'fantasy')
    kwargs.setdefault('weight', 'semibold')
    kwargs.setdefault('zorder', 50)
    #ax.text(*coords, txt, transform=ax.transAxes, **kwargs)
    ax.text(*coords, txt, **kwargs)


cd = os.path.dirname(os.path.realpath(__file__))
os.chdir(cd)
fpaths, fnames = find_files('experiments/')

lines = { fnames[i] : exclusion_line(np.loadtxt(fpaths[i])) for i in range(len(fpaths)) }

plt.figure()

# # # # # various exclusion lines

add_fill(lines['coulomb'], 'ymax', **line_props['coulomb'])
add_text('Coulomb', (7.943e-12, 2.512e-02))

add_fill(lines['cmb'], 'ymax', **line_props['cmb'])
add_text('CMB', (6.281e-13, 1.294e-04))

add_fill(lines['solarlife'], 'ymax', **line_props['solarlife'])
add_text(' Solar \nlifetime', (1.778e+02, 1.294e-09))

add_line(lines['heli1'], ls=':', lw=2, color='k', zorder=10)
add_line(lines['heli2'], ls='-', lw=2, color='k', zorder=10)
add_fill(lines['heli1'], lines['coulomb'], **line_props['heli1'])
add_text('CAST', (5.585e-04, 3.192e-05), rotation=-52)

add_fill(lines['lsw'], lines['cmb'], **line_props['lsw'])
add_fill(lines['lsw'], lines['heli1'], **line_props['lsw'])
add_text('ALPS', (3.451e-04, 7.047e-08))

add_fill(lines['hb'], 'ymax', **line_props['hb'])
add_text('HB', (3.248e+03, 9.166e-14))

add_fill(lines['rg'], 'ymax', **line_props['rg'])
add_text('RG', (4e+03, 8.427e-15))

add_fill(lines['thermal'], 'ymax', **line_props['thermal'])
add_text('Thermal Cosmology', (1.738e+05, 1.019e-05), rotation=-90)

add_fill(lines['xenon10sun'], 'ymax', **line_props['xenon10sun'])
add_text('XENON10 (solar)', (2e-03, 4e-12), rotation=-44)

add_fill(lines['halo1'], lines['cdm'], **line_props['halo1'])
add_fill(lines['halo2'], lines['cdm'], **line_props['halo2'])
add_text('Haloscopes', (6e-07, 3e-14), rotation=90)

add_line(lines['cdm'], ls='--', lw=2, color=line_props['cdm']['color'], zorder=20)
add_fill(lines['cdm'], 'ymin', **line_props['cdm'])
add_text('Allowed HP CDM', (1e-4, 2.5e-16))

add_fill(lines['tokyo'], lines['cdm'], **line_props['tokyo'])
add_text('Tokyo', (2.05, 1.5e-10), rotation=-90, color='w')

add_fill(lines['damic19'], lines['cdm'], **line_props['damic'])
add_text('DAMIC', (9.5, 3.818e-11), rotation=-90)

add_fill(lines['xenon10'], lines['cdm'], **line_props['xenon10'])
add_text('XENON10 (DM)', (1.148e+01, 2e-16))

add_fill(lines['tokyo_mev'], 'ymax', **line_props['tokyo_mev'])
add_text('Tokyo2', (5e-5, 6e-9))

add_fill(lines['tokyo_k-band'], lines['cdm'], **line_props['tokyo_k-band'])
add_text('Tokyo3', (8e-6, 3e-10))

add_fill(lines['shuket'], lines['cdm'], **line_props['shuket'])
add_text('SHUKET', (4e-5, 1e-11))

add_fill(lines['funk'], lines['cdm'], **line_props['funk'])
add_text('FUNK', (2.5, 1.1e-12), rotation=-90, color='w')

plt.xlabel(r'$\lg(m_{\tilde{\gamma}}/\text{eV})$')
plt.xscale('log')
plt.xlim(1e-7, 8e4) # (1e-13, 1e6)
plt.xticks(
    [ 10**x for x in range(-6, 5) ],
    [ str(x) if x%2==0 else str() for x in range(-6, 5) ]
)

plt.ylabel(r'$\lg\chi$')
plt.yscale('log')
plt.ylim(6e-17, 2e-8) # (1e-16, 1)
plt.yticks(
    [ 10**x for x in range(-16, -7) ],
    [ str(x) if x%2==0 else str() for x in range(-16, -7) ]
)
plt.gca().yaxis.set_minor_locator(plt.LogLocator(subs='all', numticks=10))
plt.gca().yaxis.set_minor_formatter(plt.NullFormatter())

plt.subplots_adjust(left=0.1, bottom=0.14, right=0.995, top=0.995)

plt.savefig('exclusion_limit-all.pdf')
plt.savefig('exclusion_limit-all.png', dpi=200)
plt.close()
