#!/usr/bin/env python2

import pygal, sys, re

FLOAT = '[-+]?\d+\.\d*?'
TH_TRASH = re.compile('^HS_TRASH: PAPR_TH = ('+FLOAT+'), PHPR_TH = ('+FLOAT+'), PNPR_TH = ('+FLOAT+'), IMSD = ('+FLOAT+')')
FREQ = re.compile('^Freq (\d+) Hz >\tPAPR : ('+FLOAT+') [+ ]\tPHPR : ('+FLOAT+') [+ ]\tPNPR : ('+FLOAT+') [+ ]\t?IMSD : ('+FLOAT+') [+ ]\t')

def main():
    frame = []
    for line in sys.stdin.readlines():
        res = TH_TRASH.match(line)
        if res:
            frame.append({'th' : res.groups(), 'data' : []})
        res = FREQ.match(line)
        if res:
            frame[len(frame)-1]['data'].append(res.groups())

    count = 0
    for f in frame:
        line_chart = pygal.Line(show_dots=False, width=1080, interpolate='hermite', interpolation_parameters={'type': 'cardinal', 'c': .75})
        line_chart.title = 'Frame {0}ms'.format(count*32) 
        line_chart.x_labels = map(lambda (a,b,c,d,e): a, f['data'])
        line_chart.add('PAPR', map(lambda (a,b,c,d,e): float(b), f['data']))
        line_chart.add('PHPR', map(lambda (a,b,c,d,e): float(c), f['data']))
        line_chart.add('PNPR', map(lambda (a,b,c,d,e): float(d), f['data']))
        line_chart.add('IMSD', map(lambda (a,b,c,d,e): float(e), f['data']))
        line_chart.render_to_file('img/frame_{0}.svg'.format(count))
        count = count + 1


if __name__ == '__main__':
    main()
