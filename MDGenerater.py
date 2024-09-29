import os
import sys
import json
import logging

config = {
  "template_path": '',
  "exclude_files": [],
  "exclude_folders": [],
  "output_file": '',
  "support_language": {
  }
}

outputFd = None
supportLans ={}
supportSuffix = []

def init(confPath : str) -> (bool,dict):
    configs = '{}'
    confHandle = open(confPath, 'r')
    if not confHandle.readable():
        return (False,{})
    configs = confHandle.read()
    confHandle.close()
    ret = json.loads(configs)
    return (True, ret)
    pass

def addTemplate(filePath: str, level: int, filename: str) -> None:
    content = ''; fileType = ''; prefix = '#' * level + ' '; suffix = None
    with open(filePath,'r') as fd:
        content = fd.read()
    for item in supportLans:
        if filename.endswith(item):
            fileType = supportLans[item]
            suffix = item
            break

    if suffix != None:
        title = filename[:-len(suffix)]
        title = title.lstrip('0123456789.')
        outputFd.write(F'{prefix}{title}\n')
        outputFd.write(f'\n```{fileType}\n')
        outputFd.write(content)
        outputFd.write('\n```\n')
    pass

def key(name : str) -> int:
    ret = 0
    for i in range(0,len(name)):
        if not name[i].isdigit():
            break
        ret*=10; ret+=int(name[i])
    return ret
    pass

def Quary(directory : str, level: int, title: str) -> None:
    print(f"Current Directory: {directory}")
    if title != '':
        prefix = '#' * level + ' '
        title = title.lstrip('0123456789.')
        outputFd.write(prefix + title + '\n')
    items = os.listdir(directory)

    items.sort(key=key)
    for item in items:
        item_path = os.path.join(directory, item)
        # 判断是文件还是文件夹
        if os.path.isfile(item_path):
            if not item in config['exclude_files']:
                if item.endswith(supportSuffix):
                    addTemplate(filePath= item_path, level= level+1, filename= item)
        elif os.path.isdir(item_path):
            if not item in config['exclude_folders']:
                Quary(item_path, level=level+1, title= item)
        pass

    # for root, dirs, files in os.walk(directory):
    #     # 遍历目录中的文件
    #     for file_name in files:
    #         filePath = os.path.join(root, file_name)
    #         print(f"File: {os.path.join(root, file_name)}")
    #         if not file_name in config['exclude_files']:
    #             if file_name.endswith(supportSuffix):
    #                 addTemplate(filePath= filePath, level= level+1, filename= file_name)
    #     # 遍历目录中的子目录
    #     for dir_name in dirs:
    #         next_dir = os.path.join(root, dir_name)
    #         print(f"Subdirectory: {os.path.join(root, dir_name)}")
    #         if not dir_name in config['exclude_folders']:
    #             Quary(next_dir, level=level+1, title= dir_name)
    #     break

def Work(configs : dict) -> None:
    global config
    global outputFd
    global supportLans
    global supportSuffix

    config['output_file'] = configs['output_file']
    config['template_path'] = configs['template_path']
    config['exclude_files'] = configs['exclude_files']
    config['exclude_folders'] = configs['exclude_folders']
    config['support_language'] = configs['support_language']

    if not os.path.exists(config['output_file']) :
        open(config['output_file'], 'w').close()

    supportLans = config['support_language']
    outputFd = open(config['output_file'],'w')
    supportSuffix = tuple(config['support_language'].keys())

    try:
        Quary(directory= config['template_path'], level= 0, title= '')
    except BaseException:
        print('Something error!')
    finally:
        outputFd.close()

    pass

if __name__ == '__main__':
    print('WARNING: This software has been deprecated and is no longer maintained.')
    print('Please consider using NewGenerater.py on https://github.com/RainbowXXX/AcmTemplate/blob/master/NewGenerater.py instead.')
    retry = 10; state = False; configs = {}
    while (not state) and (retry != 0):
        (state, configs) = init('./config.json')
        retry -= 1
    if not state:
        logging.error('Fail to load configs.')
    else:
        Work(configs= configs)
        pass
    
