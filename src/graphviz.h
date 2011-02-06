// Copyright 2011 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <set>

// XXX deinline all this code so we don't need this include
#include "graph.h"
struct Node;

struct GraphViz {
  void Start();
  void AddTarget(Node* node);
  void Finish();

  set<Node*> visited_;
};

void GraphViz::AddTarget(Node* node) {
  if (visited_.find(node) != visited_.end())
    return;
  printf("\"%p\" [label=\"%s\"]\n", node, node->file_->path_.c_str());
  visited_.insert(node);

  if (!node->in_edge_) {
    // Leaf node.
    // Draw as a rect?
    return;
  }

  Edge* edge = node->in_edge_;

  if (edge->inputs_.size() == 1 && edge->outputs_.size() == 1) {
    // Can draw simply.
    // Note extra space before label text -- this is cosmetic and feels
    // like a graphviz bug.
    printf("\"%p\" -> \"%p\" [label=\" %s\"]\n",
           edge->inputs_[0], edge->outputs_[0], edge->rule_->name_.c_str());
  } else {
    printf("\"%p\" [label=\"%s\", shape=ellipse]\n",
           edge, edge->rule_->name_.c_str());
    for (vector<Node*>::iterator out = edge->outputs_.begin();
         out != edge->outputs_.end(); ++out) {
      printf("\"%p\" -> \"%p\"\n", edge, *out);
    }
    for (vector<Node*>::iterator in = edge->inputs_.begin();
         in != edge->inputs_.end(); ++in) {
      const char* order_only = "";
      if (edge->is_order_only(in - edge->inputs_.begin()))
        order_only = " style=dotted";
      printf("\"%p\" -> \"%p\" [arrowhead=none%s]\n", (*in), edge, order_only);
    }
  }

  for (vector<Node*>::iterator in = edge->inputs_.begin();
       in != edge->inputs_.end(); ++in) {
    AddTarget(*in);
  }
}

void GraphViz::Start() {
  printf("digraph ninja {\n");
  printf("node [fontsize=10, shape=box, height=0.25]\n");
  printf("edge [fontsize=10]\n");
}

void GraphViz::Finish() {
  printf("}\n");
}

