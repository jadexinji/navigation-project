# C++ Map Navigation System

## 项目简介
这是一个基于 C++ 的简易地图导航系统，实现了地图显示、缩放/平移、起终点选择，以及基于 **Dijkstra** 和 **A*** 算法的路径规划和可视化功能。适合作为实习项目展示算法与地图渲染的能力。

---

## 功能说明
1. **地图加载**
   - 支持 CSV 或 Shapefile 简化地图数据
   - 节点信息：`nodes.csv`（点编号, x, y）
   - 边信息：`edges.csv`（起点编号, 终点编号, 距离）

2. **地图显示**
   - 使用 WinForms 或 GDI+ 绘制节点与道路
   - 支持鼠标缩放与平移

3. **起终点选择**
   - 鼠标点击选择起点和终点
   - 支持框选和单点选

4. **路径规划**
   - **Dijkstra 算法**：求最短路径
   - **A\*** 算法：启发式路径规划
   - 高亮显示规划路径

5. **属性/数据管理**
   - 可查看每条边的距离
   - 支持将规划结果输出为 CSV 或 JSON

---

## 目录结构

```
NavigationProject/
│
├─ MainForm.h / MainForm.cpp        # 主窗体，UI & 事件处理
├─ Graph.h / Graph.cpp              # 图数据结构 + Dijkstra/A* 算法
├─ MapRenderer.h / MapRenderer.cpp  # 绘图与缩放平移
├─ DataLoader.h / DataLoader.cpp    # CSV / Shapefile 数据读取
├─ resources/                       # 测试地图文件 (CSV/Shapefile)
└─ README.md                        # 项目说明文档
```

---

## Codex 开发任务说明

1. **读取数据**
   - 使用 `DataLoader` 读取节点和边信息
   - 建立邻接表/邻接矩阵

2. **绘制地图**
   - 在 `MapRenderer` 中绘制节点和边
   - 支持鼠标缩放和平移

3. **路径规划**
   - 实现 Dijkstra 算法
   - 实现 A* 算法（启发式函数使用欧几里得距离）
   - 高亮显示计算出的最短路径

4. **UI 交互**
   - 鼠标选择起点和终点
   - 显示路径长度和经过节点

5. **输出**
   - 将路径结果保存为 CSV 或 JSON，便于查看或进一步分析

---

## 使用说明
1. 打开 VS2022 或 VS Code（带 C++/CLI 或 WinForms 支持）  
2. 编译并运行 `MainForm`  
3. 导入 `nodes.csv` 和 `edges.csv`  
4. 鼠标选择起点/终点 → 点击“计算路径” → 查看高亮路径  

---

## 扩展建议
- 支持地图缩放到不同级别  
- 支持多条路径比较  
- 支持动态道路权重（交通、障碍物等）  

---

## 作者
- 实习项目示例  
- 使用 Codex 辅助开发
