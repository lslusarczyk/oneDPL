// SPDX-FileCopyrightText: Intel Corporation
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <oneapi/dpl/internal/distributed_ranges_impl/shp/detail.hpp>
#include <oneapi/dpl/internal/distributed_ranges_impl/shp/init.hpp>
#include <oneapi/dpl/internal/distributed_ranges_impl/shp/util.hpp>

<<<<<<< HEAD
namespace experimental::dr::shp {
=======
namespace experimental::shp {
>>>>>>> cd565891f4ffdd0b4641810a38c60c683e5f1fe0

/**
 * Applies the given function to a range and stores the result in another range,
 * beginning at out.
 * \param policy use `experimental::dr::shp::par_unseq` here only
 * \param in the range of elements to transform
 * \param out the beginning of the destination range, may be equal to the
 * beginning of `in` range \param fn operation to apply to input elements
 * \return an
 * [unary_transform_result](https://en.cppreference.com/w/cpp/algorithm/ranges/transform)
 * containing an input iterator equal to the end of `in` range and an output
 * iterator to the element past the last element transformed
 */

template <class ExecutionPolicy>
auto transform(ExecutionPolicy &&policy, experimental::dr::distributed_range auto &&in,
               experimental::dr::distributed_iterator auto out, auto &&fn) {

  static_assert( // currently only one policy supported
      std::is_same_v<std::remove_cvref_t<ExecutionPolicy>, device_policy>);

  std::vector<sycl::event> events;
  using OutT = typename decltype(out)::value_type;
  std::vector<void *> buffers;
  const auto out_end = out + rng::size(in);

  for (auto &&[in_seg, out_seg] :
       views::zip(in, rng::subrange(out, out_end)).zipped_segments()) {
    auto in_device = policy.get_devices()[in_seg.rank()];
    auto &&q = __detail::queue(experimental::dr::ranges::rank(in_seg));
    const std::size_t seg_size = rng::size(in_seg);
    assert(seg_size == rng::size(out_seg));
    auto local_in_seg = __detail::local(in_seg);

    if (in_seg.rank() == out_seg.rank()) {
      auto local_out_seg = __detail::local(out_seg);
      events.emplace_back(q.parallel_for(seg_size, [=](auto idx) {
        local_out_seg[idx] = fn(local_in_seg[idx]);
      }));
    } else {
      OutT *buffer =
          sycl::malloc_device<OutT>(seg_size, in_device, experimental::dr::shp::context());
      buffers.push_back(buffer);

      sycl::event compute_event = q.parallel_for(
          seg_size, [=](auto idx) { buffer[idx] = fn(local_in_seg[idx]); });
      events.emplace_back(q.copy(buffer, __detail::local(out_seg.begin()),
                                 seg_size, compute_event));
    }
  }
  __detail::wait(events);

  for (auto *b : buffers)
    sycl::free(b, experimental::dr::shp::context());

  return rng::unary_transform_result<decltype(rng::end(in)), decltype(out_end)>{
      rng::end(in), out_end};
}

template <experimental::dr::distributed_range R, experimental::dr::distributed_iterator Iter, typename Fn>
auto transform(R &&in, Iter out, Fn &&fn) {
  return transform(experimental::dr::shp::par_unseq, std::forward<R>(in),
                   std::forward<Iter>(out), std::forward<Fn>(fn));
}

template <typename ExecutionPolicy, experimental::dr::distributed_iterator Iter1,
          experimental::dr::distributed_iterator Iter2, typename Fn>
auto transform(ExecutionPolicy &&policy, Iter1 in_begin, Iter1 in_end,
               Iter2 out_end, Fn &&fn) {
  return transform(
      std::forward<ExecutionPolicy>(policy),
      rng::subrange(std::forward<Iter1>(in_begin), std::forward<Iter1>(in_end)),
      std::forward<Iter2>(out_end), std::forward<Fn>(fn));
}

template <experimental::dr::distributed_iterator Iter1, experimental::dr::distributed_iterator Iter2,
          typename Fn>
auto transform(Iter1 in_begin, Iter1 in_end, Iter2 out_end, Fn &&fn) {
  return transform(experimental::dr::shp::par_unseq, std::forward<Iter1>(in_begin),
                   std::forward<Iter1>(in_end), std::forward<Iter2>(out_end),
                   std::forward<Fn>(fn));
}

<<<<<<< HEAD
} // namespace experimental::dr::shp
=======
} // namespace experimental::shp
>>>>>>> cd565891f4ffdd0b4641810a38c60c683e5f1fe0
