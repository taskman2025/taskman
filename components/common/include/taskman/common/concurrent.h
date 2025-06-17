#ifndef TASKMAN_CONCURRENT_UTILITIES_INCLUDED
#define TASKMAN_CONCURRENT_UTILITIES_INCLUDED

#include <QtConcurrent>
#include <QFuture>
#include <tuple>
#include <type_traits>

template<typename Sender, typename Signal, typename... Args>
QFuture<std::tuple<std::decay_t<Args>...>> onSignal(Sender* sender, Signal signal)
{
    QPromise<std::tuple<std::decay_t<Args>...>> promise;
    QFuture<std::tuple<std::decay_t<Args>...>> future = promise.future();

    QObject::connect(sender, signal,
        [p = std::move(promise)](Args... args) mutable {
            p.addResult(std::make_tuple(std::forward<Args>(args)...));
            p.finish();
        },
        Qt::SingleShotConnection);

    return future;
}

#endif // TASKMAN_CONCURRENT_UTILITIES_INCLUDED
