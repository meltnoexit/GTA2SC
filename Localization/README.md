# 《侠盗猎车手2》简体中文汉化 - 本地化文件

> [!TIP]
> 本项目使用 **[uv](https://docs.astral.sh/uv/)** 管理 Python 环境及项目依赖。

## 构建

### 安装依赖


```
uv sync --no-dev
```

### 构建项目

> [!IMPORTANT]
> 构建前，请先下载 [Unifont](https://www.unifoundry.com/unifont/) 字体，并将 `unifont-*.hex` 文件放入 `font/` 目录。


```
uv run --no-dev python build.py
```

> [!NOTE]
> 构建产物位于 `build/` 目录，包括：
>
> - `bob_j.gxt`：GTA2 Manager 文本文件
> - `j.gxt`：GTA2 游戏文本文件
> - `kanji.dat`：GTA2 游戏字库文件
>
> 将上述文件放入`游戏目录/data/` 目录即可使用。

## 开发

### 安装依赖

```
uv sync
```

### 代码检查


```
uv run ruff check .
```

### 代码格式化

```
uv run ruff format .
```