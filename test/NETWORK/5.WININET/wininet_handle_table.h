#ifndef _WININET_HANDLE_TABLE_H_
#define _WININET_HANDLE_TABLE_H_

#include "wininet_handle_base.h"

namespace toc {
namespace tuk2 {
using WininetHandle = void*;

class WininetHandleTable {
 public:
  static WininetHandleTable* getHandleTable() {
    if (!handle_table_)
      handle_table_ = new WininetHandleTable();
    return handle_table_;
  }

  /// @brief get Object from wininet Handle
  WininetBase* getObjectFromHandle(WininetHandle handle);

  /// @brief Create Rootsession of Wininet handle
  WininetRootSession* createRootObject(WininetHandleType type) {
    std::shared_ptr<WininetRootSession> obj(new WininetRootSession());
    obj->setIndex(handle_list_.size());
    handle_list_.push_back(obj);
    return obj.get();
  }
  /// @brief Create Wininet handle (except Rootsession)
  /// Template method
  template <class WininetType>
  WininetType* createObject(WininetBase* parent) {
    std::shared_ptr<WininetType> obj(new WininetType(parent));
    obj->setIndex(handle_list_.size());
    handle_list_.push_back(obj);
    return obj.get();
  }

  /// @close wininet object
  int closeObject(WininetHandle handle);

 private:
  WininetHandleTable() {}
  ~WininetHandleTable() {}
  static WininetHandleTable* handle_table_;

 private:
  std::vector<std::shared_ptr<WininetBase>> handle_list_;
};

} /* namespace tuk2 */
} /* namespace toc */

#endif /* _WININET_HANDLE_TABLE_H_ */
