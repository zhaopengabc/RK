// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common/sample_common.h"
#include "rkmedia_api.h"
#include "rkmedia_vdec.h"

char *g_filename = "/userdata/test.h264";
RK_U32 g_disp1_width = 1920;
RK_U32 g_disp1_height = 1080;
RK_U32 g_is_hardware = 1;
IMAGE_TYPE_E g_enPixFmt = IMAGE_TYPE_NV12;
CODEC_TYPE_E g_enCodecType = RK_CODEC_TYPE_H264;
#define INBUF_SIZE 4096

static bool quit = false;
static void sigterm_handler(int sig) {
  fprintf(stderr, "signal %d\n", sig);
  quit = true;
}

static RK_CHAR optstr[] = "?:p:f:w:h:t:";
static void print_usage() {
  printf("usage example: rkmedia_vdec_test -w 720 -h 480 -p /userdata/out.jpeg "
         "-f 0 -t JPEG.\n");
  printf("\t-w: width, Default: 1280\n");
  printf("\t-h: height, Default: 720\n");
  printf("\t-p: file path, Default: /userdata/test.h264\n");
  printf("\t-f: 1:hardware 0:software, Default:hardware\n");
  printf("\t-t: codec type, Default H264, support JPEG and H264.\n");
}
int ret = 0;
int main(int argc, char *argv[]) {
  int c;
  while ((c = getopt(argc, argv, optstr)) != -1) {
    switch (c) {
    case 'w':
      g_disp1_width = atoi(optarg);
      break;
    case 'h':
      g_disp1_height = atoi(optarg);
      break;
    case 'p':
      g_filename = optarg;
      break;
    case 'f':
      g_is_hardware = atoi(optarg);
      break;
    case 't':
      if (strcmp(optarg, "H264") == 0) {
        g_enCodecType = RK_CODEC_TYPE_H264;
      } else if (strcmp(optarg, "JPEG") == 0) {
        g_enCodecType = RK_CODEC_TYPE_JPEG;
      }
      break;
    case '?':
    default:
      print_usage();
      return 0;
    }
  }

  signal(SIGINT, sigterm_handler);
  RK_MPI_SYS_Init();
  FILE *f = fopen(g_filename, "rb");
  if (!f) {
    fprintf(stderr, "Could not open %s\n", g_filename);
    return 0;
  }
  MPP_CHN_S VdecChn, VoChn;
  VdecChn.enModId = RK_ID_VDEC;
  VdecChn.s32DevId = 0;
  VdecChn.s32ChnId = 0;
  VoChn.enModId = RK_ID_VO;
  VoChn.s32DevId = 0;
  VoChn.s32ChnId = 0;

  // VDEC
  VDEC_CHN_ATTR_S stVdecAttr;
  stVdecAttr.enCodecType = g_enCodecType;
  // stVdecAttr.enImageType = g_enPixFmt;
  stVdecAttr.enMode = VIDEO_MODE_FRAME;
  if (g_is_hardware) 
  {
    if (stVdecAttr.enCodecType == RK_CODEC_TYPE_JPEG) 
    {
      stVdecAttr.enMode = VIDEO_MODE_FRAME;
    } 
    else
    {
      stVdecAttr.enMode = VIDEO_MODE_STREAM;
    }
    stVdecAttr.enDecodecMode = VIDEO_DECODEC_HADRWARE;
  } 
  else 
  {
    stVdecAttr.enMode = VIDEO_MODE_FRAME;
    stVdecAttr.enDecodecMode = VIDEO_DECODEC_SOFTWARE;
  }
  printf("stVdecAttr.enCodecType : %d \n",stVdecAttr.enCodecType);
  // printf("stVdecAttr.enImageType : %d \n",stVdecAttr.enImageType);
  printf("g_is_hardware : %d \n",g_is_hardware);

  printf("stVdecAttr.enMode : %d \n",stVdecAttr.enMode);
  printf("stVdecAttr.enDecodecMode : %d \n",stVdecAttr.enDecodecMode);


  ret = RK_MPI_VDEC_CreateChn(VdecChn.s32ChnId, &stVdecAttr);
  printf("RK_MPI_VDEC_CreateChn return : %d \n",ret);
  printf("g_disp1_width: %d , g_disp1_height : %d \n",g_disp1_width,g_disp1_height);

  // VO
  VO_CHN_ATTR_S stVoAttr = {0};
  memset(&stVoAttr, 0, sizeof(stVoAttr));
  stVoAttr.pcDevNode = "/dev/dri/card0";
  stVoAttr.emPlaneType = VO_PLANE_PRIMARY;
  stVoAttr.enImgType = g_enPixFmt;
  stVoAttr.u16Zpos = 1;
  stVoAttr.stImgRect.s32X = 0;
  stVoAttr.stImgRect.s32Y = 0;
  stVoAttr.stImgRect.u32Width = g_disp1_width;
  stVoAttr.stImgRect.u32Height = g_disp1_height;
  stVoAttr.stDispRect.s32X = 0;
  stVoAttr.stDispRect.s32Y = 0;
  stVoAttr.stDispRect.u32Width = g_disp1_width ;
  stVoAttr.stDispRect.u32Height = g_disp1_height;
  ret = RK_MPI_VO_CreateChn(VoChn.s32ChnId, &stVoAttr);
  printf("ret : 0x%x \n",ret);
  // VDEC->VO
  RK_MPI_SYS_Bind(&VdecChn, &VoChn);

  if (stVdecAttr.enMode == VIDEO_MODE_STREAM) {
    size_t data_size;
    MEDIA_BUFFER mb =
        RK_MPI_MB_CreateBuffer(INBUF_SIZE + 64, RK_TRUE, MB_FLAG_NOCACHED);

    //  MEDIA_BUFFER out_MB =
    //     RK_MPI_MB_CreateBuffer(INBUF_SIZE + 64, RK_TRUE, MB_FLAG_NOCACHED);
    while (!feof(f)) 
    {
      //  read raw data from the input file 
      data_size = fread(RK_MPI_MB_GetPtr(mb), 1, INBUF_SIZE, f);

      // printf("data_size : %d \n",data_size);
      RK_MPI_MB_SetSzie(mb, data_size);
      if (!data_size)
        break;
      // printf("send on to vdec, %p.\n", RK_MPI_MB_GetPtr(mb));
      RK_MPI_SYS_SendMediaBuffer(VdecChn.enModId, VdecChn.s32ChnId, mb);
      usleep(30 * 1000);
      //  RK_MPI_MB_ReleaseBuffer(mb);
      // out_MB = RK_MPI_SYS_GetMediaBuffer(VoChn.enModId,VoChn.s32ChnId,1000);
      // if(out_MB == NULL)
      // {
      //   printf("read meida buffer is NULL \n");
      //   break;
      // }
    }
    RK_MPI_MB_ReleaseBuffer(mb);
    // RK_MPI_MB_ReleaseBuffer(out_MB);
  } else if (stVdecAttr.enMode == VIDEO_MODE_FRAME) {
    size_t data_size;
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    MEDIA_BUFFER mb = RK_MPI_MB_CreateBuffer(length, RK_TRUE, MB_FLAG_NOCACHED);
    data_size = fread(RK_MPI_MB_GetPtr(mb), 1, length, f);
    RK_MPI_MB_SetSzie(mb, data_size);
    RK_MPI_SYS_SendMediaBuffer(VdecChn.enModId, VdecChn.s32ChnId, mb);
    usleep(30 * 1000);
    RK_MPI_MB_ReleaseBuffer(mb);
  }

  while (!quit) {
    usleep(10 * 1000);
  }
  RK_MPI_SYS_UnBind(&VdecChn, &VoChn);
  RK_MPI_VDEC_DestroyChn(VdecChn.s32ChnId);
  RK_MPI_VO_DestroyChn(VoChn.s32ChnId);
  fclose(f);

  return 0;
}
