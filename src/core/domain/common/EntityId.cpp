#include "src/core/domain/common/EntityId.h"

#include <QUuid>

#include <utility>

namespace geovideo::domain {

EntityId::EntityId(QString value)
    : value_(std::move(value))
{
}

EntityId EntityId::create()
{
    // 使用 UUID 作為 domain entity 的穩定識別碼，避免資料庫主鍵和 UI 暫存資料混在一起。
    return EntityId(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

EntityId EntityId::fromString(const QString& value)
{
    // 從資料庫或設定檔讀回時，先修掉前後空白，避免肉眼看不出的 id 錯誤。
    return EntityId(value.trimmed());
}

bool EntityId::isValid() const
{
    // 目前只檢查非空字串，之後若要嚴格驗證 UUID 格式，可以集中在這裡調整。
    return !value_.trimmed().isEmpty();
}

QString EntityId::toString() const
{
    return value_;
}

bool EntityId::operator==(const EntityId& other) const
{
    return value_ == other.value_;
}

bool EntityId::operator!=(const EntityId& other) const
{
    return !(*this == other);
}

} // namespace geovideo::domain