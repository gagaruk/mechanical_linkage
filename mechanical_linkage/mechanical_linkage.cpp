#include <iostream>
#include <math.h>

//program object definitions

//entities
struct point_t{
  double x;
  double y;
  double global_angle; 
  double local_angle;
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
  linkage_slot_t*** tail = head;
};
struct evaluation_order_t{
  int capacity = 128;
  linkage_slot_t** head = new linkage_slot_t*[capacity];
  int count = 0;

  int* dependency_orders = new int[capacity];

};

struct display_t{
  static constexpr double x_min = -45.0;
  static constexpr double x_max = 45.0;
  static constexpr double y_min = -45.0;
  static constexpr double y_max = 45.0;

  static const int width = 90;
  static const int height = 30;
  static const int character_width = 3;

  static const int logical_width = width/character_width;
  
  inline static char buffer[static_cast<int>(width/character_width)*height]; //I know I could I used a string but the goal is keeping it low level u know
};
struct t_matrix{
  double m[3][3];
};

//program functions
template <typename T>
void resize_array(T*& array_ptr, int& current_capacity, double resize_factor);

void create_point(int x, int y);
void create_linkage(int parent_point, int child_point, link_type type, char render_char);
  
void delete_node(int index);
void delete_node(int point_a, int point_b);
void delete_node(linkage_slot_t** table_addr);

point_slot_t* allocate_node(int x, int y);
linkage_slot_t* allocate_node(int parent_point, int child_point, link_type, char render_char);

void deallocate_node(point_slot_t*);
void deallocate_node(linkage_slot_t*);

void add_to_table(point_slot_t* new_point);
linkage_slot_t** add_to_table(linkage_slot_t* new_linkage);

void push_to_gap(point_slot_t** table_slot);
void push_to_gap(linkage_slot_t** table_slot);

point_slot_t** pop_from_gap(const point_indirectionTable_t& table);
linkage_slot_t** pop_from_gap(const linkage_indirectionTable_t& table);

void add_to_evaluation(linkage_slot_t** linkage);
void delete_from_evaluation(linkage_slot_t** linkage);
void regenerate_evaluation_order();

void add_points_to_buffer();
void add_linkages_to_buffer();
void flush_buffer();
void clear_buffer();

void display_nodes();
void apply_transformation(int point, const t_matrix& matrix);
void calculate_forward_kinematics();

void reset();
void reset_points();
void reset_linkages();

//program objects
point_indirectionTable_t pointsTable;
point_gap_slots_t point_gaps;

linkage_indirectionTable_t linkagesTable;
linkage_gap_slots_t linkage_gaps;

point_page_t* point_pageStack = new point_page_t;
linkage_page_t* linkage_pageStack = new linkage_page_t;

evaluation_order_t evaluation_order;

display_t display;

int main(){
  clear_buffer();

  std::string inputString;
  int op;

  while(true){
    std::cout << "\nOperations:\n";
    std::cout << "0. Add point\n"; //complete* ->asterisk means tested against edge cases
    std::cout << "1. Remove point\n"; //complete*

    std::cout << "2. Add linkage\n"; //complete*
    std::cout << "3. Delete linkage\n"; //complete*

    std::cout << "4. Display nodes\n"; //complete*

    std::cout << "5. Apply Transformation\n";
    std::cout << "6. Reset\n" << std::endl;

    std::cout << "Enter operation number: ";  
    std::getline(std::cin, inputString);

    
    op = (inputString.size() == 1 && std::isdigit(inputString[0])) ? std::stoi(inputString) : -1;

    if(op < 0 || op > 6 ){
      std::cout<<op;
      std::cout << "Invalid operation number.\n";
      std::cout << "Please enter a valid operation number (0, 1, 2, 3, 4, 5, 6): ";
      std::cout << "\n" << std::endl;
      continue;
    }

    try{
      switch(op){
        case 0:{
          int x;
          int y;

          std::string x_str;
          std::string y_str;

          std::cout << "Add point operation selected.\n\n"; 
          std::cout << "Choose enter coordinates" << std::endl;

          std::cout << "X: ";
          std::getline(std::cin, x_str);
          std::cout << "Y: ";
          std::getline(std::cin, y_str);

          x = std::stoi(x_str);
          y = std::stoi(y_str);

          if(x>display.x_max || y>display.y_max || x<display.x_min || y<display.y_min){
            std::cout << "X or Y value exceeds the world limitations:\n " 
            << "X_min= " << display.x_min << ",  X_max= " << display.x_max << "| Y_min= " << display.y_min << ",  Y_max= " << display.y_max << std::endl;
            continue;
          }
          create_point(x, y);
          break;
        }
        case 1:{
          std::string idx_str;
          int idx;

          std::cout << "Remove point operation selected.\n\n";
          std::cout << "Select point(index): ";
          std::getline(std::cin, idx_str);

          for(int i=0; i<idx_str.size();i++){
            if(!std::isdigit(idx_str[i])){
              std::cout << "Wrong entry redirecting to the main menu" << std::endl;
              continue;
            }
          }
          idx = std::stoi(idx_str);
          delete_node(idx);
          break;
        }
        case 2:{
          std::string parent_str;
          std::string child_str;
          std::string type_str;
          std::string render_char_str;
          
          int parent_idx;
          int child_idx;
          link_type type;
          char render_char;

          std::cout << "Add linkages operation selected.\n\n";
          std::cout << "Determine properties:" << std::endl;

          std::cout << "Parent point(idx): ";
          std::getline(std::cin, parent_str);
          std::cout << "Child point(idx): ";
          std::getline(std::cin, child_str);
          std::cout << "Render char:";
          std::getline(std::cin, render_char_str);
          std::cout << "Linkage Types: 0.Visual | 1.Rotational | 2.Rigid\n";
          std::cout << "Enter the type-number: ";
          std::getline(std::cin, type_str);

          parent_idx = std::stoi(parent_str);
          child_idx = std::stoi(child_str);

          int max_point_idx = pointsTable.tail-pointsTable.head;
          
          if (parent_idx < 0 || parent_idx >= max_point_idx || pointsTable.head[parent_idx] == nullptr) {
            std::cout << "Error: Parent point index " << parent_idx << " does not exist or has been deleted.\n\n";
            continue;
          }

          if (child_idx < 0 || child_idx >= max_point_idx || pointsTable.head[child_idx] == nullptr) {
            std::cout << "Error: Child point index " << child_idx << " does not exist or has been deleted.\n\n";
            continue;
          }

          else if(parent_idx==child_idx){
            std::cout << "Cannot create linkage connecting point to itself";
          }
          

          type = static_cast<link_type>(std::stoi(type_str));
          if(type<0 or type>2){
            std::cout << "Linkage type only accepts values 0, 1, 2";
            continue;
          }

          render_char = (render_char_str.empty()) ? '*' : render_char_str[0];
          create_linkage(parent_idx, child_idx, type, render_char);
          break;
        }  
        case 3:{
          std::string point_a_str;
          std::string point_b_str;

          int point_a_idx;
          int point_b_idx;

          std::cout << "Delete Linkage operation selected\n";
          std::cout << "Enter linkage points:" << std::endl;

          std::cout << "Point A(index): ";
          std::getline(std::cin, point_a_str);
          std::cout << "Point B(index): ";
          std::getline(std::cin, point_b_str);
          
          point_a_idx = std::stoi(point_a_str);
          point_b_idx = std::stoi(point_b_str);

          delete_node(point_a_idx,point_b_idx);
          
          break;
        }
        case 4:{
          std::cout << "Display nodes operation selected.\n\n";
          display_nodes();
          break;
        }
        case 5:{
          std::string point_str;
          t_matrix matrix;

          std::cout << "Apply Transformation Matrix selected.\n";
          std::cout << "Enter target Point Index: ";
          std::getline(std::cin, point_str);
          int pt_idx = std::stoi(point_str);

          std::cout << "Enter 3x3 Matrix coefficients row by row:\n";
          for (int r = 0; r < 3; r++) {
              for (int c = 0; c < 3; c++) {
                  std::string val_str;
                  std::cout << "m[" << r << "][" << c << "]: ";
                  std::getline(std::cin, val_str);
                  matrix.m[r][c] = std::stod(val_str);
              }
          }
          matrix.m[2][0] = 0.0;
          matrix.m[2][1] = 0.0;
          matrix.m[2][2] = 1.0;

          apply_transformation(pt_idx, matrix);
          std::cout << "Transformation successfully applied!\n";
          break;
        }
        case 6:{
          std::cout << "Reseting the system";
          reset();
          std::cout << "Reset successfully made";
          break;
        }
        default: 
          break;
      }
    }
    catch(...){
      std::cout<<"\nWritten something wrong to an input field\n" << std::endl;
    }
  }
  return 0;
}

template <typename T>
void resize_array(T*& array_ptr, int& current_capacity, double resize_factor){
  int new_capacity = (current_capacity == 0) ? 8 : current_capacity * resize_factor;

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
  bool exists = false;
  int possible_linkages= linkagesTable.tail - linkagesTable.head;
  for(int i=0;i<possible_linkages;i++){
    if(linkagesTable.head[i]==nullptr) continue;  
    if(linkagesTable.head[i]->data.child==child_point && linkagesTable.head[i]->data.parent==parent_point ){
      std::cout << "This linkage already exists";
      exists = true;
      break;
    }
    if(linkagesTable.head[i]->data.child==parent_point && linkagesTable.head[i]->data.parent==child_point){
      std::cout << "A linkage with reversed roles exist, deleting the old one";
      delete_node(linkagesTable.head+i);
      break;
    }
  }
  if(!exists){
    linkage_slot_t* slot = allocate_node(parent_point, child_point, type, render_char);
    linkage_slot_t** linkage = add_to_table(slot);
    add_to_evaluation(linkage);
  }
}

void delete_node(int index){
  point_slot_t** table_addr = pointsTable.head+index;
  if(*table_addr != nullptr && index<=pointsTable.tail-pointsTable.head){
    deallocate_node(*table_addr);
    *table_addr = nullptr;
    push_to_gap(table_addr);

    int possible_linkage_count = linkagesTable.tail - linkagesTable.head;
    for(int i=0; i<possible_linkage_count;i++){
      if(linkagesTable.head[i]->data.child == index || linkagesTable.head[i]->data.parent == index){
        delete_node(linkagesTable.head + i);
        break;
      }
    }
  }else{
    std::cout << " No such point exists";
  }
}
void delete_node(int point_a, int point_b){
  int possible_linkage_count = linkagesTable.tail - linkagesTable.head;
  linkage_slot_t** table_addr = nullptr;

  for(int i=0;i < possible_linkage_count;i++){
    if(linkagesTable.head[i]==nullptr) continue;  
    if((linkagesTable.head[i]->data.child == point_a && linkagesTable.head[i]->data.parent == point_b) || (linkagesTable.head[i]->data.child == point_b && linkagesTable.head[i]->data.parent == point_a)){
      table_addr = linkagesTable.head+i;
      break;
    }
  }
  if(table_addr != nullptr){
    delete_node(table_addr);
    std::cout << "Linkage successfully deleted.\n";
  }else{
    std::cout << "No such linkage exists.\n";
  }
}
void delete_node(linkage_slot_t** table_addr){
  deallocate_node(*table_addr);
    *table_addr = nullptr;
    push_to_gap(table_addr);
}

point_slot_t* allocate_node(int x, int y){
  point_slot_t* allocation_addr;
  point_page_t* current_point_page;
  if(point_pageStack->count == 0 && point_pageStack->prev != nullptr && point_pageStack->prev->count < point_pageStack->prev->capacity){
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
  if(linkage_pageStack->count == 0 && linkage_pageStack->prev != nullptr && linkage_pageStack->prev->count < linkage_pageStack->prev->capacity){
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

  double global_angle = (dy == 0.0 && dx == 0.0) ? 0: std::atan2(dy, dx);
  allocation_addr->data.length = std::sqrt((dx * dx) + (dy * dy));
  allocation_addr->data.base_angle = global_angle;

  pointsTable.head[child_point]->data.global_angle = global_angle;
  pointsTable.head[child_point]->data.local_angle = global_angle - pointsTable.head[parent_point]->data.global_angle;

  return allocation_addr;
}
void deallocate_node(point_slot_t* point){

  point->next_free = point_pageStack->top_free;
  point_pageStack->top_free = point;

  point_page_t* point_Page = nullptr;
  point_page_t* current_point_page = point_pageStack;
  point_page_t* parent_of_page = nullptr;

  while(current_point_page != nullptr){
    if(point >= current_point_page->slots && point < (current_point_page->slots + current_point_page->capacity)){
      point_Page = current_point_page;
      break;
    }
    parent_of_page = current_point_page;
    current_point_page = current_point_page->prev;
  }
  if (point_Page != nullptr) {
    point_Page->count--;

    if(point_Page->count == 0) {
      if(point_Page == point_pageStack) {
        if(point_pageStack->prev != nullptr) {
          point_page_t* page_to_delete = point_pageStack;
          point_pageStack = point_pageStack->prev;
          delete page_to_delete;
        }
      } else {
        if(parent_of_page != nullptr) {
          parent_of_page->prev = point_Page->prev;
          delete point_Page;
        }
      }
    }
  }
}

void deallocate_node(linkage_slot_t* linkage){
  linkage->next_free = linkage_pageStack->top_free;
  linkage_pageStack->top_free = linkage;

  linkage_page_t* linkage_Page = nullptr;
  linkage_page_t* current_linkage_page = linkage_pageStack;
  linkage_page_t* parent_of_page = nullptr;

  while(current_linkage_page != nullptr){
    if(linkage >= current_linkage_page->slots && linkage < (current_linkage_page->slots + current_linkage_page->capacity)){
      linkage_Page = current_linkage_page;
      break;
    }
    parent_of_page = current_linkage_page;
    current_linkage_page = current_linkage_page->prev;
  }

  if (linkage_Page != nullptr) {
    linkage_Page->count--;

    if(linkage_Page->count == 0) {
      if(linkage_Page == linkage_pageStack) {
        if(linkage_pageStack->prev != nullptr) {
          linkage_page_t* page_to_delete = linkage_pageStack;
          linkage_pageStack = linkage_pageStack->prev;
          delete page_to_delete;
        }
      } else {
        if(parent_of_page != nullptr) {
          parent_of_page->prev = linkage_Page->prev;
          delete linkage_Page;
        }
      }
    }
  }
}

void add_to_table(point_slot_t* new_point){
  if(point_gaps.head != point_gaps.tail){
    point_slot_t** table_addr = pop_from_gap(pointsTable);
    *table_addr = new_point;
  }else{
    *pointsTable.tail = new_point;
    pointsTable.tail++;
  }
}
linkage_slot_t** add_to_table(linkage_slot_t* new_linkage){
  linkage_slot_t** table_addr;
  if(linkage_gaps.head != linkage_gaps.tail){
    table_addr = pop_from_gap(linkagesTable);
  }else{
    table_addr = linkagesTable.tail;
    linkagesTable.tail++;
  }
  *table_addr = new_linkage;
  return table_addr;
}

void push_to_gap(point_slot_t** table_slot){
  if(point_gaps.tail - point_gaps.head >= point_gaps.capacity){
    resize_array(point_gaps.head, point_gaps.capacity, 2.0);
  }
  *point_gaps.tail = table_slot;
  point_gaps.tail++;
}
void push_to_gap(linkage_slot_t** table_slot){
  if(linkage_gaps.tail - linkage_gaps.head >= linkage_gaps.capacity){
    resize_array(linkage_gaps.head, linkage_gaps.capacity, 2.0);
  }
  *linkage_gaps.tail = table_slot;
  linkage_gaps.tail++;
}

point_slot_t** pop_from_gap(const point_indirectionTable_t& table){

  int active_elements = point_gaps.tail - point_gaps.head;
  if(active_elements <= point_gaps.capacity*0.25){
    resize_array(point_gaps.head, point_gaps.capacity, 0.5);
    point_gaps.tail = point_gaps.head + active_elements;
  }
  point_gaps.tail--;
  return *point_gaps.tail;
}
linkage_slot_t** pop_from_gap(const linkage_indirectionTable_t& table){
  int active_elements = linkage_gaps.tail - linkage_gaps.head;
  if(active_elements<= linkage_gaps.capacity*0.25){
    resize_array(linkage_gaps.head, linkage_gaps.capacity, 0.5);
    linkage_gaps.tail = linkage_gaps.head + active_elements;
  }
  linkage_gaps.tail--;
  return *linkage_gaps.tail;
}

void add_to_evaluation(linkage_slot_t** linkage){
  if(evaluation_order.count == evaluation_order.capacity){
    int old_capacity = evaluation_order.capacity;
    resize_array(evaluation_order.head, evaluation_order.capacity, 2.0);
    
    // Remember to reallocate your parallel tracking dependencies array too!
    int* new_deps = new int[evaluation_order.capacity];
    for(int i = 0; i < old_capacity; i++) {
        new_deps[i] = evaluation_order.dependency_orders[i];
    }
    delete[] evaluation_order.dependency_orders;
    evaluation_order.dependency_orders = new_deps;
  }
  regenerate_evaluation_order();
}
void delete_from_evaluation(linkage_slot_t** linkage){
  if(evaluation_order.count < evaluation_order.capacity * 0.25){
    int old_capacity = evaluation_order.capacity;
    resize_array(evaluation_order.head, evaluation_order.capacity, 0.5);
    
    int* new_deps = new int[evaluation_order.capacity];
    for(int i = 0; i < evaluation_order.capacity; i++) {
        new_deps[i] = evaluation_order.dependency_orders[i];
    }
    delete[] evaluation_order.dependency_orders;
    evaluation_order.dependency_orders = new_deps;
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

  int* zero_dep_q= new int[possible_point_count];
  int q_head = 0;
  int q_tail = 0;

  for (int i = 0; i < possible_point_count; ++i) {
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

    calculate_forward_kinematics();
}

void clear_buffer(){
  for(int i=0; i<display.width*display.height;i++){
    display.buffer[i] = ' ';
  }
}
void add_points_to_buffer(){
  for(int i=0;i<pointsTable.tail-pointsTable.head;i++){
    if(pointsTable.head[i] != nullptr){
      int x_pixel = static_cast<int>((pointsTable.head[i]->data.x-display.x_min)/(display.x_max-display.x_min)*((display.logical_width)-1));
      int y_pixel = static_cast<int>((pointsTable.head[i]->data.y-display.y_min)/(display.y_max-display.y_min)*(display.height - 1));
      
      if (x_pixel < 0 || x_pixel >= display.logical_width || y_pixel < 0 || y_pixel >= display.height) {
        continue;
      }

      int base_index = (y_pixel*display.width)+(x_pixel*display.character_width)+1; // +1 -> the base charachter shows the middle of the symbol

      if(i>=100){
        display.buffer[base_index-1] = (i/100)+'0';
        display.buffer[base_index] =((i-(i/100))/10)+'0';
        display.buffer[base_index+1] = (i%10)+'0';
      }else if(i>=10){
        display.buffer[base_index-1] = '[';
        display.buffer[base_index] = (i/10)+'0';
        display.buffer[base_index+1] = (i%10)+'0';
      }else{
        display.buffer[base_index-1] = '[';
        display.buffer[base_index] = i+'0';
        display.buffer[base_index+1] = ']';
      }
    }
  }
}
void add_linkages_to_buffer(){
  for(int i = 0; i < linkagesTable.tail - linkagesTable.head; i++){
    if(linkagesTable.head[i] == nullptr){
      continue;
    }
    linkage_slot_t* linkage = linkagesTable.head[i];
    int point_a = linkage->data.child;
    int point_b = linkage->data.parent;

    if (pointsTable.head[point_a] == nullptr || pointsTable.head[point_b] == nullptr) {
      continue;
    }
    double world_x0 = pointsTable.head[point_a]->data.x;
    double world_y0 = pointsTable.head[point_a]->data.y;
    double world_x1 = pointsTable.head[point_b]->data.x;
    double world_y1 = pointsTable.head[point_b]->data.y;

    int x0 = static_cast<int>((world_x0 - display.x_min) / (display.x_max - display.x_min) * (display.logical_width - 1));
    int y0 = static_cast<int>((world_y0 - display.y_min) / (display.y_max - display.y_min) * (display.height - 1));
    int x1 = static_cast<int>((world_x1 - display.x_min) / (display.x_max - display.x_min) * (display.logical_width - 1));
    int y1 = static_cast<int>((world_y1 - display.y_min) / (display.y_max - display.y_min) * (display.height - 1));

    // 2. Set up Alois Zingl's Symmetric Bresenham Algorithm
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < display.logical_width && y0 >= 0 && y0 < display.height) {
            int base_idx = (y0 * display.width) + (x0 * display.character_width)+1;// +1 -> the base charachter shows the middle of the symbol

            
            if (display.buffer[base_idx] != '[' && display.buffer[base_idx] != ']' && !std::isdigit(display.buffer[base_idx])) {
                display.buffer[base_idx-1] = ' ';
                display.buffer[base_idx] = linkage->data.render_char;
                display.buffer[base_idx+1] = ' ';
            }
        }
        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
  }
}
void flush_buffer(){
  for(int i=0;i<display.width*display.height;i++){
    std::cout << display.buffer[i];
    if((i+1)%display.width == 0 && i != 0){
      std::cout << '\n';
    }
  }
  std::cout << std::endl;  
}
void display_nodes(){
  clear_buffer();
  add_points_to_buffer();
  add_linkages_to_buffer();
  flush_buffer();
}

void apply_tranformation(int point_idx, const t_matrix& matrix){
  int max_point_idx = pointsTable.tail - pointsTable.head;
  if(point_idx<0 || point_idx>max_point_idx || pointsTable.head[point_idx]==nullptr){
    std::cout << "Invalid point index";
    return;
  }

  point_t& pt = pointsTable.head[point_idx]->data;

  double new_x = pt.x * matrix.m[0][0] + pt.y * matrix.m[0][1] + 1.0 * matrix.m[0][2];
  double new_y = pt.x * matrix.m[1][0] + pt.y * matrix.m[1][1] + 1.0 * matrix.m[1][2];

  pt.x = new_x;
  pt.y = new_y;

  regenerate_evaluation_order();
}
void calculate_forward_kinematics(){ //to be used in regenerate evaluation order
  for(int i=0; i<evaluation_order.count; i++){
    linkage_t& linkage = evaluation_order.head[i]->data;  //use references when able to ptr 2nd choice (refs cant be reassigned nor null)

    point_t& parent_pt = pointsTable.head[linkage.parent]->data;
    point_t& child_pt = pointsTable.head[linkage.child]->data;

    if(linkage.type == RIGID){
      child_pt.global_angle = parent_pt.global_angle + child_pt.local_angle;
      
      child_pt.x = parent_pt.x + linkage.length * std::cos(child_pt.global_angle);
      child_pt.x = child_pt.x + linkage.length * std::sin(child_pt.global_angle);
    }
    else if(linkage.type == ROTATIONAL){
      child_pt.global_angle = parent_pt.global_angle + linkage.base_angle;

      child_pt.x = parent_pt.x + linkage.length * std::cos(child_pt.global_angle);
      child_pt.x = child_pt.x + linkage.length * std::sin(child_pt.global_angle);
    }
    else if(linkage.type == VISUAL){
      double dx = parent_pt.x - child_pt.x;
      double dy = parent_pt.y - child_pt.y;
      
      linkage.length = std::sqrt((dx*dx)+(dy*dy));
      linkage.base_angle = (dx==0.0 ||(dx==0.0 && dy==0.0)) ? 0.0 : std::atan2(dy,dx);
    }
  }
}

void reset(){
  reset_points();
  reset_linkages();
}
void reset_points(){
  point_page_t* current_page = point_pageStack;
  while(current_page != nullptr){
    point_page_t* next_to_delete = current_page->prev;
    delete current_page;
    current_page = next_to_delete;
  }

  point_pageStack = new point_page_t;

  delete[] pointsTable.head;
  pointsTable.capacity = 128; // initial cap.
  pointsTable.head = new point_slot_t*[pointsTable.capacity];
  pointsTable.tail = pointsTable.head;

  delete[] point_gaps.head;
  point_gaps.capacity = 128; // initial cap.
  point_gaps.head = new point_slot_t**[point_gaps.capacity];
  point_gaps.tail = point_gaps.head;
}
void reset_linkages(){
  linkage_page_t* current_page = linkage_pageStack;
  while(current_page != nullptr){
    linkage_page_t* next_to_delete = current_page->prev;
    delete current_page;
    current_page = next_to_delete;
  }

  linkage_pageStack = new linkage_page_t;

  delete[] linkagesTable.head;
  linkagesTable.capacity = 128; // init cap.
  linkagesTable.head = new linkage_slot_t*[linkagesTable.capacity];
  linkagesTable.tail = linkagesTable.head;

  delete[] linkage_gaps.head;
  linkage_gaps.capacity = 128; // init cap.
  linkage_gaps.head = new linkage_slot_t**[linkage_gaps.capacity];
  linkage_gaps.tail = linkage_gaps.head;

  delete[] evaluation_order.head;
  delete[] evaluation_order.dependency_orders;

  evaluation_order.capacity = 128; // inti cap.
  evaluation_order.head = new linkage_slot_t*[evaluation_order.capacity];
  evaluation_order.dependency_orders = new int[evaluation_order.capacity];
  evaluation_order.count = 0;
}