import os
import codecs

# 配置你需要转换的文件夹路径，'.' 表示当前目录及所有子目录
TARGET_DIR = '.' 

def convert_encoding(file_path, from_enc='gbk', to_enc='utf-8'):
    try:
        # 尝试以原始编码读取
        with codecs.open(file_path, 'r', encoding=from_enc) as f:
            content = f.read()
        # 以新编码写入
        with codecs.open(file_path, 'w', encoding=to_enc) as f:
            f.write(content)
        print(f"转换成功: {file_path}")
    except UnicodeDecodeError:
        # 如果报错，说明可能已经是UTF-8了，或者包含无法识别的字符
        print(f"跳过 (可能已是UTF-8或非GBK): {file_path}")
    except Exception as e:
        print(f"转换失败 {file_path}: {e}")

for root, dirs, files in os.walk(TARGET_DIR):
    for file in files:
        if file.endswith('.c') or file.endswith('.h'):
            file_path = os.path.join(root, file)
            convert_encoding(file_path)

print("批量转换完成！")