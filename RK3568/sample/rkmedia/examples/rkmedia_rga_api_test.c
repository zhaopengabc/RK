// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <rga/im2d.h>
#include <rga/rga.h>

#ifdef RKAIQ
#include "common/sample_common.h"
#endif

#include "rkmedia_api.h"
#include "rkmedia_venc.h"

typedef struct rga_demo_arg_s {
  RK_U32 target_x;
  RK_U32 target_y;
  RK_U32 target_width;
  RK_U32 target_height;
  RK_U32 vi_width;
  RK_U32 vi_height;
} rga_demo_arg_t;

static bool quit = false;
static void sigterm_handler(int sig) {
  fprintf(stderr, "signal %d\n", sig);
  quit = true;
}

static void *GetMediaBuffer(void *arg) {
  printf("#Start %s thread, arg:%p\n", __func__, arg);
  rga_demo_arg_t *crop_arg = (rga_demo_arg_t *)arg;
  int ret;
  rga_buffer_t src;
  rga_buffer_t dst;
  MEDIA_BUFFER src_mb = NULL;
  MEDIA_BUFFER dst_mb = NULL;
  printf("test, %d, %d, %d, %d\n", crop_arg->target_height,
         crop_arg->target_width, crop_arg->vi_height, crop_arg->vi_width);
  while (!quit) {
    src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI, 0, -1);
    if (!src_mb) {
      printf("ERROR: RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
      break;
    }

    MB_IMAGE_INFO_S stImageInfo = {
        crop_arg->target_width, crop_arg->target_height, crop_arg->target_width,
        crop_arg->target_height, IMAGE_TYPE_NV12};
    dst_mb = RK_MPI_MB_CreateImageBuffer(&stImageInfo, RK_TRUE, 0);
    if (!dst_mb) {
      printf("ERROR: RK_MPI_MB_CreateImageBuffer get null buffer!\n");
      break;
    }

    src = wrapbuffer_fd(RK_MPI_MB_GetFD(src_mb), crop_arg->vi_width,
                                 crop_arg->vi_height, RK_FORMAT_YCbCr_420_SP);
    dst =
        wrapbuffer_fd(RK_MPI_MB_GetFD(dst_mb), crop_arg->target_width,
                               crop_arg->target_height, RK_FORMAT_YCbCr_420_SP);
    im_rect src_rect = {crop_arg->target_x, crop_arg->target_y,
                        crop_arg->target_width, crop_arg->target_height};
    im_rect dst_rect = {0};
    ret = imcheck(src, dst, src_rect, dst_rect, IM_CROP);
    if (IM_STATUS_NOERROR != ret) {
      printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
      break;
    }
    IM_STATUS STATUS = imcrop(src, dst, src_rect);
    if (STATUS != IM_STATUS_SUCCESS)
      printf("imcrop failed: %s\n", imStrError(STATUS));

    RK_MPI_SYS_SendMediaBuffer(RK_ID_VO, 0, dst_mb);
    RK_MPI_MB_ReleaseBuffer(src_mb);
    RK_MPI_MB_ReleaseBuffer(dst_mb);
    src_mb = NULL;
    dst_mb = NULL;
  }

  if (src_mb)
    RK_MPI_MB_ReleaseBuffer(src_mb);
  if (dst_mb)
    RK_MPI_MB_ReleaseBuffer(dst_mb);

  return NULL;
}

static RK_CHAR optstr[] = "?::a::x:y:d:H:W:w:h:";
static const struct option long_options[] = {
    {"aiq", optional_argument, NULL, 'a'},
    {"vi_height", required_argument, NULL, 'H'},
    {"vi_width", required_argument, NULL, 'W'},
    {"crop_height", required_argument, NULL, 'h'},
    {"crop_width", required_argument, NULL, 'w'},
    {"device_name", required_argument, NULL, 'd'},
    {"crop_x", required_argument, NULL, 'x'},
    {"crop_y", required_argument, NULL, 'y'},
    {"help", optional_argument, NULL, '?'},
    {NULL, 0, NULL, 0},
};

static void print_usage(const RK_CHAR *name) {
  printf("usage example:\n");
#ifdef RKAIQ
  printf("\t%s "
         "[-a [iqfiles_dir]] "
         "[-H 1920] "
         "[-W 1080] "
         "[-h 640] "
         "[-w 640] "
         "[-x 300] "
         "[-y 300] "
         "[-d rkispp_scale0] \n",
         name);
  printf("\t-a | --aiq: enable aiq with dirpath provided, eg:-a "
         "/oem/etc/iqfiles/, "
         "set dirpath empty to using path by default. without this option, aiq "
         "should run in other application\n");
#else
  printf("\t%s [-H 1920] "
         "[-W 1080] "
         "[-h 640] "
         "[-w 640] "
         "[-x 300] "
         "[-y 300] "
         "[-d rkispp_scale0] \n",
         name);
#endif
  printf("\t-H | --vi_height: VI height, Default:1080\n");
  printf("\t-W | --vi_width: VI width, Default:1920\n");
  printf("\t-h | --crop_height: crop_height, Default:640\n");
  printf("\t-w | --crop_width: crop_width, Default:640\n");
  printf("\t-x  | --crop_x: start x of cropping, Default:300\n");
  printf("\t-y  | --crop_y: start y of cropping, Default:300\n");
  printf("\t-d  | --device_name set pcDeviceName, Default:rkispp_scale0\n");
  printf("\t  option: [rkispp_scale0, rkispp_scale1, rkispp_scale2]\n");
  printf("Notice: fmt always NV12\n");
}

int main(int argc, char *argv[]) {
  int ret = 0;
  rga_demo_arg_t demo_arg;
  memset(&demo_arg, 0, sizeof(rga_demo_arg_t));
  demo_arg.target_width = 640;
  demo_arg.target_height = 640;
  demo_arg.vi_width = 1920;
  demo_arg.vi_height = 1080;
  demo_arg.target_x = 300;
  demo_arg.target_y = 300;
  char *device_name = "rkispp_scale0";
  char *iq_file_dir = NULL;
  int c = 0;
  opterr = 1;
  while ((c = getopt_long(argc, argv, optstr, long_options, NULL)) != -1) {
    const char *tmp_optarg = optarg;
    switch (c) {
    case 'a':
      if (!optarg && NULL != argv[optind] && '-' != argv[optind][0]) {
        tmp_optarg = argv[optind++];
      }
      if (tmp_optarg) {
        iq_file_dir = (char *)tmp_optarg;
      } else {
        iq_file_dir = "/oem/etc/iqfiles/";
      }
      break;
    case 'H':
      demo_arg.vi_height = atoi(optarg);
      break;
    case 'W':
      demo_arg.vi_width = atoi(optarg);
      break;
    case 'h':
      demo_arg.target_height = atoi(optarg);
      break;
    case 'w':
      demo_arg.target_width = atoi(optarg);
      break;
    case 'x':
      demo_arg.target_x = atoi(optarg);
      break;
    case 'y':
      demo_arg.target_y = atoi(optarg);
      break;
    case 'd':
      device_name = optarg;
      break;
    case '?':
    default:
      print_usage(argv[0]);
      return 0;
    }
  }

  printf("device_name: %s\n\n", device_name);
  printf("#vi_height: %d\n\n", demo_arg.vi_height);
  printf("#vi_width: %d\n\n", demo_arg.vi_width);
  printf("#crop_x: %d\n\n", demo_arg.target_x);
  printf("#crop_y: %d\n\n", demo_arg.target_y);
  printf("#crop_height: %d\n\n", demo_arg.target_height);
  printf("#crop_width: %d\n\n", demo_arg.target_width);

  if (demo_arg.vi_height < (demo_arg.target_height + demo_arg.target_y) ||
      demo_arg.vi_width < (demo_arg.target_width + demo_arg.target_x)) {
    printf("crop size is over vi\n");
    return -1;
  }

  if (iq_file_dir) {
#ifdef RKAIQ
    printf("#Aiq xml dirpath: %s\n\n", iq_file_dir);
    rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
    RK_BOOL fec_enable = RK_FALSE;
    int fps = 30;
    SAMPLE_COMM_ISP_Init(hdr_mode, fec_enable, iq_file_dir);
    SAMPLE_COMM_ISP_Run();
    SAMPLE_COMM_ISP_SetFrameRate(fps);
#endif
  }

  RK_MPI_SYS_Init();
  VI_CHN_ATTR_S vi_chn_attr;
  vi_chn_attr.pcVideoNode = device_name;
  vi_chn_attr.u32BufCnt = 3;
  vi_chn_attr.u32Width = demo_arg.vi_width;
  vi_chn_attr.u32Height = demo_arg.vi_height;
  vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
  vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
  ret = RK_MPI_VI_SetChnAttr(0, 0, &vi_chn_attr);
  ret |= RK_MPI_VI_EnableChn(0, 0);
  if (ret) {
    printf("ERROR: Create vi[0] failed! ret=%d\n", ret);
    return -1;
  }
  printf("type is %d\n", VO_PLANE_OVERLAY);

  VO_CHN_ATTR_S stVoAttr = {0};
  stVoAttr.pcDevNode = "/dev/dri/card0";
  stVoAttr.emPlaneType = VO_PLANE_OVERLAY;
  stVoAttr.enImgType = IMAGE_TYPE_NV12;
  stVoAttr.u16Zpos = 1;
  stVoAttr.u16Fps = 60;
  stVoAttr.stImgRect.s32X = 0;
  stVoAttr.stImgRect.s32Y = 0;
  stVoAttr.stImgRect.u32Width = demo_arg.target_width;
  stVoAttr.stImgRect.u32Height = demo_arg.target_height;
  stVoAttr.stDispRect.s32X = 0;
  stVoAttr.stDispRect.s32Y = 0;
  stVoAttr.stDispRect.u32Width = demo_arg.target_width;
  stVoAttr.stDispRect.u32Height = demo_arg.target_height;
  ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
  if (ret) {
    printf("Create vo[0] failed! ret=%d\n", ret);
    return -1;
  }

  pthread_t read_thread;
  pthread_create(&read_thread, NULL, GetMediaBuffer, &demo_arg);

  usleep(1000); // waite for thread ready.
  ret = RK_MPI_VI_StartStream(0, 0);
  if (ret) {
    printf("ERROR: Start Vi[0] failed! ret=%d\n", ret);
    return -1;
  }

  printf("%s initial finish\n", __func__);
  signal(SIGINT, sigterm_handler);
  while (!quit) {
    usleep(500000);
  }

  printf("%s exit!\n", __func__);
  RK_MPI_VO_DestroyChn(0);
  RK_MPI_VI_DisableChn(0, 0);

  if (iq_file_dir) {
#ifdef RKAIQ
    SAMPLE_COMM_ISP_Stop();
#endif
  }

  return 0;
}
