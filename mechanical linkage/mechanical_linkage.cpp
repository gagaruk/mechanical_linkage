#include <iostream>
#include <math.h>

//entities
struct point_t{
  double x,y;
  double global_angle, local_angle = 0;
};

enum link_type{
  VISUAL,
  ROTATIONAL,
  RIGID
};

struct linkage_t{
  int parent;
  int child;
  link_type type;

  double length;
  double  base_angle;
  char render_char;
};

//entity storage
union point_slot_t{
  point_t data;
  point_slot_t* next_free;
};

struct point_page_t{
  static const int capacity = 64;
  int count = 0;
  point_slot_t slots[capacity];
  point_page_t* prev = nullptr;
  point_slot_t* top_free = nullptr;
};

struct point_indirectionTable_t{
  int capacity = 128;
  point_slot_t** head = new point_slot_t*[capacity];
  point_slot_t** tail = head;
};

struct point_gap_slots_t{
  int capacity = 64;
  point_slot_t*** head = new point_slot_t**[capacity];
  point_slot_t*** tail = head;
};

union linkage_slot_t{ //same as point_slot 
  linkage_t data;
  linkage_slot_t* next_free;
};

struct linkage_page_t{ //same as point_page 
  static const int capacity = 64;
  int count = 0;
  linkage_slot_t slots[capacity];
  linkage_page_t* prev = nullptr;
  linkage_slot_t* top_free = nullptr; 
};

struct linkage_indirectionTable_t{ //same as point_indirectionTable 
  int capacity = 128;
  linkage_slot_t** head = new linkage_slot_t*[capacity];
  linkage_slot_t** tail = head;
};

struct linkage_gap_slots_t{ // same as linkage_indirectionTable
  int capacity = 64;
  linkage_slot_t*** head = new linkage_slot_t**[capacity];
  linkage_slot_t*** tail = new linkage_slot_t**[capacity];
};

struct evaluation_order_t{
  int capacity = 128;
  linkage_slot_t** head = new linkage_slot_t*[capacity];
  int count = 0;

  int* dependency_orders = new int[capacity];

};

template <typename T>
void resize_array(T*& array_ptr, int& current_capacity, double resize_factor);

void create_point(int x, int y);
void create_linkage(point_slot_t* parent_point, point_slot_t* child_point, link_type type);
  

void delete_node(int index);
void delete_node(int point_a, int point_b);

point_slot_t* allocate_node(int x, int y);
linkage_slot_t* allocate_node(int parent_point, int child_point, link_type, char render_char);

void deallocate_node(point_slot_t*);
void deallocate_node(linkage_slot_t*);


void add_to_table(point_slot_t* new_point);
linkage_slot_t** add_to_table(linkage_slot_t* new_linkage);

void push_to_gap(point_slot_t** table_slot);
void push_to_gap(linkage_slot_t** table_slot);

point_slot_t** pop_from_gap(point_indirectionTable_t* table);
linkage_slot_t** pop_from_gap(linkage_indirectionTable_t* table);

void add_to_evaluation(linkage_slot_t** linkage);
void delete_from_evaluation(linkage_slot_t** linkage);
void regenerate_evaluation_order();

point_indirectionTable_t pointsTable;
point_gap_slots_t point_gaps;

linkage_indirectionTable_t linkagesTable;
linkage_gap_slots_t linkage_gaps;

point_page_t* point_pageStack = new point_page_t{};
linkage_page_t* linkage_pageStack = new linkage_page_t{};

evaluation_order_t evaluation_order;


int main(){
  std::string inputString;
  int op;

  std::cout << "Operations:\n";
  std::cout << "1. Add point\n";
  std::cout << "2. Remove point\n";
  std::cout << "3. Display points\n\n";

  std::cout << "Enter operation number: ";  
  std::getline(std::cin, inputString);
  op = std::stoi(inputString);


  while(op < 1 || op > 3){
    std::cout << "Invalid operation number.\n";
    std::cout << "Please enter a valid operation number (1, 2, or 3): ";
    std::getline(std::cin, inputString);
    op = std::stoi(inputString);
  }
  switch(op){
    case 1:{
      std::string x;
      std::string y;

      std::cout << "Add point operation selected.\n\n"; 
      std::cout << "Choose enter coordinates" << std::endl;

      std::cout << "X: ";
      std::getline(std::cin, x);
      std::cout << "Y: ";
      std::getline(std::cin, y);

      create_point(std::stoi(x), std::stoi(y));

      break;
    }
    case 2:{
      std::string index;
      std::cout << "Remove point operation selected.\n\n";

      std::cout << "Select point(index): ";
      std::getline(std::cin, index);
      
      delete_node(std::stoi(index));
      break;
    }
    case 3:
      std::cout << "Display points operation selected.\n\n";
      break;
    default: 
      break;
  }


  return 0;
}

template <typename T>
void resize_array(T*& array_ptr, int& current_capacity, double resize_factor){
  static const int _growth_factor=2;

  int new_capacity = (current_capacity == 0) ? 8 : current_capacity * _growth_factor;

  T* new_array = new T[new_capacity];
  for(int i=0; i<current_capacity;i++){
    *(new_array+i) = *(array_ptr+i);
  }

  delete[] array_ptr;
  array_ptr = new_array;
  current_capacity = new_capacity;
}

void create_point(int x, int y){
  point_slot_t*  slot = allocate_node(x, y);
  add_to_table(slot);
}
void create_linkage(int parent_point, int child_point, link_type type, char render_char){
  linkage_slot_t* slot = allocate_node(parent_point, child_point, type, render_char);
  linkage_slot_t** linkage = add_to_table(slot);
  add_to_evaluation(linkage);
}

void delete_node(int index){
  point_slot_t** table_addr = pointsTable.head+index;
  deallocate_node(*table_addr);
  *table_addr = nullptr;
  push_to_gap(table_addr);
}
void delete_node(int point_a, int point_b){
  int possible_linkage_count = linkagesTable.tail - linkagesTable.head;
  linkage_slot_t** table_addr;

  for(int i=0;i < possible_linkage_count;i++){
    if((linkagesTable.head[i]->data.child == point_a && linkagesTable.head[i]->data.parent == point_b) || (linkagesTable.head[i]->data.child == point_b && linkagesTable.head[i]->data.parent == point_a)){
      table_addr = linkagesTable.head+i;
    }
  }
  deallocate_node(*table_addr);
  *table_addr = nullptr;
  push_to_gap(table_addr);
}

point_slot_t* allocate_node(int x, int y){
  point_slot_t* allocation_addr;
  point_page_t* current_point_page;
  if(point_pageStack->count = 0 && point_pageStack->prev->count < point_pageStack->prev->capacity){
    current_point_page = point_pageStack->prev;
  }else{
    current_point_page = point_pageStack;
  }

  if(point_pageStack->top_free != nullptr){
      allocation_addr = point_pageStack->top_free;
      point_pageStack->top_free = point_pageStack->top_free->next_free;
  }
  else{
    if(current_point_page->count >= current_point_page->capacity){
      point_page_t* new_point_page = new point_page_t{};
      new_point_page->prev = point_pageStack;
      new_point_page->top_free = point_pageStack->top_free;
      point_pageStack = new_point_page;
    }

    allocation_addr = current_point_page->slots + current_point_page->count; 
  }

  current_point_page->count++;

  allocation_addr->data.x = x;
  allocation_addr->data.y = y;

  return allocation_addr;
}
linkage_slot_t* allocate_node(int parent_point, int child_point, link_type type, char render_char){
  linkage_slot_t* allocation_addr;
  linkage_page_t* current_linkage_page;
  if(linkage_pageStack->count = 0 && linkage_pageStack->prev->count < linkage_pageStack->prev->capacity){
    current_linkage_page = linkage_pageStack->prev;
  }else{
    current_linkage_page = linkage_pageStack;
  }

  if(linkage_pageStack->top_free != nullptr){
      allocation_addr = linkage_pageStack->top_free;
      linkage_pageStack->top_free = linkage_pageStack->top_free->next_free;
  }
  else{
    if(current_linkage_page->count >= current_linkage_page->capacity){
      linkage_page_t* new_linkage_page = new linkage_page_t{};
      new_linkage_page->prev = linkage_pageStack;
      new_linkage_page->top_free = linkage_pageStack->top_free;
      linkage_pageStack = new_linkage_page;
    }

    allocation_addr = current_linkage_page->slots + current_linkage_page->count; 
  }

  current_linkage_page->count++;

  allocation_addr->data.parent = parent_point;
  allocation_addr->data.child = child_point;
  allocation_addr->data.type = type;
  allocation_addr->data.render_char = render_char;

  double dy = pointsTable.head[child_point]->data.y - pointsTable.head[parent_point]->data.y;
  double dx = pointsTable.head[child_point]->data.x - pointsTable.head[parent_point]->data.x;

  double global_angle = std::atan2(dy, dx);
  allocation_addr->data.length = std::sqrt((dx * dx) + (dy * dy));
  allocation_addr->data.base_angle = global_angle;

  pointsTable.head[child_point]->data.global_angle = global_angle;
  pointsTable.head[child_point]->data.local_angle = global_angle - pointsTable.head[parent_point]->data.global_angle;

  return allocation_addr;
}

void deallocate_node(point_slot_t* point){
  point->next_free = point_pageStack->top_free;
  point_pageStack->top_free = point;

  point_page_t* point_Page;
  point_page_t* current_point_page = point_pageStack;
  while(current_point_page->prev != nullptr){
    if(point>current_point_page->slots && point<(current_point_page->slots+ current_point_page->capacity)){
      point_Page = current_point_page;
      break;
    }
  }
  current_point_page->count--;
  if(current_point_page->count==0 && current_point_page!=point_pageStack){
    delete[] current_point_page->slots;
    delete current_point_page;
  }else if(current_point_page->count==0 && current_point_page==point_pageStack && current_point_page->prev->count == 0){
    delete[] current_point_page->slots;
    delete current_point_page;
  }
}
void deallocate_node(linkage_slot_t* linkage){
  linkage->next_free = linkage_pageStack->top_free;
  linkage_pageStack->top_free = linkage;

  linkage_page_t* linkage_Page;
  linkage_page_t* current_linkage_page = linkage_pageStack;
  while(current_linkage_page->prev != nullptr){
    if(linkage>current_linkage_page->slots && linkage<(current_linkage_page->slots+ current_linkage_page->capacity)){
      linkage_Page = current_linkage_page;
      break;
    }
  }
  current_linkage_page->count--;
  if(current_linkage_page->count==0 && current_linkage_page!=linkage_pageStack){
    delete[] current_linkage_page->slots;
    delete current_linkage_page;
  }else if(current_linkage_page->count==0 && current_linkage_page==linkage_pageStack && current_linkage_page->prev->count == 0){
    delete[] current_linkage_page->slots;
    delete current_linkage_page;
  }
}

void add_to_table(point_slot_t* new_point){
  if(point_gaps.head != point_gaps.tail){
    point_slot_t** table_addr = pop_from_gap(&pointsTable);
    *table_addr = new_point;
  }else{
    *pointsTable.tail = new_point;
    pointsTable.tail++;
  }
}
linkage_slot_t** add_to_table(linkage_slot_t* new_linkage){
  if(linkage_gaps.head != linkage_gaps.tail){
    linkage_slot_t** table_addr = pop_from_gap(&linkagesTable);
    *table_addr = new_linkage;
  }else{
    *linkagesTable.tail = new_linkage;
    linkagesTable.tail++;
  }
}

void push_to_gap(point_slot_t** table_slot){
  if(point_gaps.head - point_gaps.tail >= point_gaps.capacity){
    resize_array(point_gaps.head, point_gaps.capacity, 2.0);
    point_gaps.tail = point_gaps.head + point_gaps.capacity;
  }
  *point_gaps.tail = table_slot;
  point_gaps.tail++;
}
void push_to_gap(linkage_slot_t** table_slot){
  if(linkage_gaps.head - linkage_gaps.tail >= linkage_gaps.capacity){
    resize_array(linkage_gaps.head, linkage_gaps.capacity, 2.0);
    linkage_gaps.tail = linkage_gaps.head + linkage_gaps.capacity;
  }
  *linkage_gaps.tail = table_slot;
  linkage_gaps.tail++;
}

point_slot_t** pop_from_gap(const point_indirectionTable_t& table){
  if(point_gaps.tail - point_gaps.head <= point_gaps.capacity*0.25){
    resize_array(point_gaps.head, point_gaps.capacity, 0.5);
    point_gaps.tail = point_gaps.head + point_gaps.capacity;
  }
  point_gaps.tail--;
  return *point_gaps.tail;
}
linkage_slot_t** pop_from_gap(const linkage_indirectionTable_t& table){
  if(linkage_gaps.tail - linkage_gaps.head <= linkage_gaps.capacity*0.25){
    resize_array(linkage_gaps.head, linkage_gaps.capacity, 0.5);
    linkage_gaps.tail = linkage_gaps.head + linkage_gaps.capacity;
  }
  linkage_gaps.tail--;
  return *linkage_gaps.tail;
}

void add_to_evaluation(linkage_slot_t** linkage){
  if(evaluation_order.count = evaluation_order.capacity){
    resize_array(evaluation_order.head, evaluation_order.capacity, 2);
  }
  regenerate_evaluation_order();
}

void delete_from_evaluation(linkage_slot_t** linkage){
  if(evaluation_order.count < evaluation_order.capacity*0.25){
    resize_array(evaluation_order.head, evaluation_order.capacity, 0.5);
  }
  regenerate_evaluation_order();
}

void regenerate_evaluation_order(){
  evaluation_order.count = 0;

  int possible_point_count = pointsTable.tail-pointsTable.head;
  int active_point_count = possible_point_count - (point_gaps.tail-point_gaps.head);
  
  int possible_linkage_count = linkagesTable.tail-linkagesTable.head;

  int* parent_counts = new int[possible_point_count] {0};

  for(int i = 0; i<possible_linkage_count;i++){
    linkage_slot_t* linkage = *(linkagesTable.head+i);
    if(linkage==nullptr) continue;
    
    (*(parent_counts+linkage->data.child))++;

    int parent_idx = linkage->data.parent;
    int child_idx  = linkage->data.child;

    if (pointsTable.head[parent_idx] == nullptr || 
            pointsTable.head[child_idx] == nullptr) {
            continue; 
    }
  }

  int* zero_dep_q= new int[active_point_count];
  int q_head = 0;
  int q_tail = 0;

  for (int i = 0; i < active_point_count; ++i) {
        if (pointsTable.head[i] != nullptr && parent_counts[i] == 0) {
            zero_dep_q[q_tail++] = i;
        }
  }
  while (q_head < q_tail) {
        int current_point_idx = zero_dep_q[q_head++];

        for (int i = 0; i < possible_linkage_count; ++i) {
            linkage_slot_t* link_slot = linkagesTable.head[i];
            if (link_slot == nullptr) continue;

            linkage_t& linkage = link_slot->data;

            if (linkage.parent == current_point_idx) {
                int order_idx = evaluation_order.count;
                evaluation_order.head[order_idx] = link_slot;
                
                evaluation_order.dependency_orders[order_idx] = current_point_idx;
                evaluation_order.count++;

                int child_idx = linkage.child;
                parent_counts[child_idx]--;

                if (parent_counts[child_idx] == 0) {
                    zero_dep_q[q_tail++] = child_idx;
                }
            }
        }
    }

    delete[] parent_counts;
    delete[] zero_dep_q;
}
