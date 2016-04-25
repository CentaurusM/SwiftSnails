//
//  worker/terminate.h
//  SwiftSnails
//
//  Created by Chunwei on 3/29/15.
//  Copyright (c) 2015 Chunwei. All rights reserved.
//
#pragma once
#include "../../../utils/all.h"
#include "../../transfer/transfer.h"
#include "../message_classes.h"
#include "../ServerWorkerRoute.h"
#include "../../parameter/global_param_cache.h"

namespace swift_snails {

template <class Key, class Val, class Grad> class ClientTerminate {
public:
  typedef Key key_t;
  typedef Val val_t;
  typedef Grad grad_t;
  typedef Transfer<ServerWorkerRoute> transfer_t;

  explicit ClientTerminate()
      : gtransfer(global_transfer<ServerWorkerRoute>()) {}

  void operator()() {
    // terminate_deamons();
    worker_send_finish_message();
    RAW_LOG(WARNING, "###################################");
    RAW_LOG(WARNING, "     Worker terminate normally");
    RAW_LOG(WARNING, "###################################");
  }

protected:
  // send finish message to master
  void worker_send_finish_message() {
    RAW_LOG(WARNING, ".. worker_send_finish_message");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    Request req;
    req.meta.message_class = WORKER_FINISH_WORK;
    req.call_back_handler = [this](std::shared_ptr<Request> response) {
      _wait_rsp_barrier.set_state_valid();
      _wait_rsp_barrier.try_unblock();
    };
    req.cont << 1234;
    gtransfer.send(std::move(req), 0);
    _wait_rsp_barrier.block();

    RAW_LOG(WARNING, "worker terminate!");
  }

private:
  typedef GlobalParamCache<key_t, val_t, grad_t> param_cache_t;
  transfer_t &gtransfer;
  StateBarrier _wait_rsp_barrier;

}; // class ClientTerminate

}; // end namespace swift_snails
