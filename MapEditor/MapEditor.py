import json
from json import JSONDecodeError

import easygui
key_name = ['type', 'begin_time', 'end_time', 'begin_locate', 'end_locate', 'rotate']
default_element = ['        ', '0:0:0000', '0:0:0000', '(0,0)', '(0,0)', '0']
main_file = 'map.json'
# 每个图式，由type, begin_time, end_time, begin_locate, end_locate, rotate组成


def add_element(last_element, element_list):
    le = last_element
    title = '地图元素添加'
    message = '请添加元素'
    fields = ['类型', '开始时间', '结束时间', '开始坐标(x,y)', '结束坐标(x,y)', '旋转角度（逆时针为负)']
    values = [le['type'], le['begin_time'], le['end_time'], le['begin_locate'], le['end_locate'], le['rotate']]
    return_value = easygui.multenterbox(title, message, fields, values)
    if None != return_value:
        element_list.append(dict(zip(key_name, return_value)))


def delete_element(element_list):
    message = '类型 开始时间 结束时间 开始坐标 结束坐标 旋转角度\n'
    title = '删除组件'
    choices0 = element_list
    choices = []
    for element in choices0:
        choices.append(json.dumps(element))
    res = easygui.multchoicebox(msg=message, title=title, choices=choices)
    for element in res:
        del element_list[element_list.index(json.loads(element))]


with open(main_file, "r", encoding='utf-8') as f:
    try:
        element_map = json.load(f)
    except JSONDecodeError:
        element_map = []
    flag = True
    while flag:
        if len(element_map) > 1:
            last_element = element_map[len(element_map)-1]
        else:
            last_element = dict(zip(key_name, default_element))
        title = '地图编辑器'
        message = '类型 开始时间 结束时间 开始坐标 结束坐标 旋转角度\n'
        for element in element_map:
            message = message + ' '.join(element.values()) + '\n'
        res = easygui.buttonbox(msg=message, title=title, choices=['添加组件', '删除组件', '保存并退出'],
                                default_choice='保存并退出')
        if res == '添加组件':
            add_element(last_element, element_map)
        if res == '保存并退出':
            flag = False
        if res == '删除组件':
            delete_element(element_map)
with open(main_file, 'w', encoding='utf-8') as f:
    json.dump(element_map, f)
