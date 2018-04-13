#include <stdio.h>

#include "wininet_handle_table.h"

namespace toc {
namespace tuk2 {

WininetHandleTable* WininetHandleTable::handle_table_ = nullptr;

WininetBase* WininetHandleTable::getObjectFromHandle(WininetHandle handle) {
  for (auto& it : handle_list_) {
    if (it.get()->getIndex() == reinterpret_cast<int>(handle)) {
      return it.get();
    }
  }
  return nullptr;
}
int WininetHandleTable::closeObject(WininetHandle handle) {
  for (auto& it : handle_list_) {
    if (it.get()->getIndex() == reinterpret_cast<int>(handle)) {
      it.get()->setInvalid();
      //      it.reset(); /FIXME : 메모리 해제 관련 검토 */
      return 0;
    }
  }
  return -1;
}
} /* namespace tuk2 */
} /* namespace toc */
