# labuladong C 本地调试模板

这套模板补齐了官方 `lc-plugin-template` 缺失的 C 语言本地调试环境，适用于 VS Code 和 Cursor。它包含：

- C11 + GCC/GDB 的一键编译和 F5 断点调试；
- LeetCode `ListNode`、`TreeNode` 的构造、打印和释放工具；
- 与本机练习项目一致的 `VosVector / VOS_Vector*` API；
- 与本机练习项目一致的 `VosPriQue / VOS_PriQue*` API；
- `VOS_VECTOR` 和历史拼写 `VOS_PROPRIQUEUE` 兼容类型；
- 官方 uthash 2.4.0 单头文件库。

## 首次配置

当前安装的 `labuladong.leetcode-helper 3.4.4` 虽然能选择 C 刷题，但它的“Setup Debug Template”命令只允许 Java、C++、Python、Go 和 JavaScript，内部没有 C 映射。请把 `labuladong-settings.json` 中的四项合并到 VS Code 的**用户级** `settings.json`，然后重新加载窗口。

关键配置结果应为：

```json
"labuladong-leetcode.defaultLanguage": "c",
"labuladong-leetcode.workspaceFolder": "C:\\Users\\ZQD\\Desktop\\labuladong\\lc-plugin-template\\c-template"
```

插件生成的 C 文件会进入：

```text
leetcode/editor/cn/<题目名>.c
leetcode/editor/en/<problem-name>.c
```

打开生成的题目文件后：

- `Ctrl + Shift + B`：编译当前 C 文件；
- `F5`：编译并用 GDB 调试当前 C 文件；
- 在题目文件底部的 `main` 中写本地测试。

本机配置已经指向：

```text
D:\BaiduNetdiskDownload\mingw64\bin\gcc.exe
D:\BaiduNetdiskDownload\mingw64\bin\gdb.exe
```

## CSTL 用法

### VosVector / VOS_VECTOR

```c
VosVector *vector = VOS_VectorCreate(sizeof(int));
int value = 42;

VOS_VectorPushBack(vector, &value);
printf("%d\n", *(int *)VOS_VectorAt(vector, 0));
VOS_VectorDestroy(vector, NULL);
```

完整接口包括 `Create / RawCreate / PushBack / At / Size / Erase / Clear / Sort / Search / Destroy`。

### VosPriQue / VOS_PROPRIQUEUE

比较器返回正数表示左参数优先级更高，因此 `VOS_IntCmpFunc` 创建的是大顶堆：

```c
VOS_PROPRIQUEUE *queue = VOS_PriQueCreate(VOS_IntCmpFunc, NULL);

VOS_PriQuePush(queue, (uintptr_t)7);
VOS_PriQuePush(queue, (uintptr_t)3);
VOS_PriQuePush(queue, (uintptr_t)9);
printf("%zu\n", (size_t)VOS_PriQueTop(queue)); /* 9 */

VOS_PriQueDestroy(queue);
```

如果队列保存指针并注册了 `dupFunc/freeFunc`，`Push` 会复制输入，`Pop/Clear/Destroy` 会释放队列拥有的数据。`PushBatch` 要求注册 `dupFunc`，以免保存临时数组里的悬空地址。

## uthash 用法

`lc_common.h` 已经包含官方 `uthash.h`。也可以显式写：

```c
#include "uthash.h"
```

示例见 `leetcode/editor/cn/two-sum.c`，覆盖了 `HASH_FIND_INT`、`HASH_ADD_INT`、`HASH_ITER` 和 `HASH_DEL`。

重要：加入 uthash 后，节点地址必须保持稳定。不要把带 `UT_hash_handle` 的结构体按值放进会扩容搬家的 Vector 或优先队列；请单独 `malloc` 节点，容器中只保存节点指针。

## 本地与在线提交的边界

插件只提交 `// @lc code=start` 与 `// @lc code=end` 之间的内容，所以本地 `main` 和 `../common/lc_common.h` 不会上传。

- LeetCode 的 C 环境默认提供 uthash，可在解题区直接使用 uthash 宏；
- `VosVector`、`VosPriQue` 是本地辅助库，在线判题环境不会自动获得它们。若解题函数依赖这些类型，需要改为原生 C 实现后再提交，或把所需实现合并进提交区。

## 自检

在 PowerShell 中运行：

```powershell
.\tools\verify.ps1
```

脚本会以 `-std=c11 -Wall -Wextra -Wpedantic -Werror` 编译并运行 CSTL、uthash、链表、树和两道示例题。
