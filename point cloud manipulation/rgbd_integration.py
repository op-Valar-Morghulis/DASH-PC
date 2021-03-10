import glob, os
import numpy as np
import open3d as o3d


def main(data_dir, spots, output_file):
    volume = o3d.integration.ScalableTSDFVolume(
            voxel_length=20.0 / 512.0,
            #sdf_trunc=0.04,
            sdf_trunc=0.1,
            color_type=o3d.integration.TSDFVolumeColorType.RGB8)

    for spot in spots:
        print(spot)
        common_prefix = os.path.join(data_dir, spot)
        for cam_id in range(3):
            cam_file = '%s_intrinsics_%d.txt'%(common_prefix, cam_id)
            width, height, fx, fy, cx, cy = np.loadtxt(cam_file)[:6]
            width = int(width)
            height = int(height)
            cam_intrinsic = o3d.camera.PinholeCameraIntrinsic(width, height, fx, fy, cx, cy)
            for angle_id in range(6):
                f_color = '%s_i%d_%d.jpg'%(common_prefix, cam_id, angle_id)
                f_depth = '%s_d%d_%d.png'%(common_prefix, cam_id, angle_id)
                color_img = o3d.io.read_image(f_color)
                depth_img = o3d.io.read_image(f_depth)
                pose_file = '%s_pose_%d_%d.txt'%(common_prefix, cam_id, angle_id)
                pose = np.loadtxt(pose_file)
                pose[:3, 3] *= 4
                rgbd_image = o3d.geometry.RGBDImage.create_from_color_and_depth(
                                color_img, depth_img, depth_trunc=10.0,
                                convert_rgb_to_intensity=False)
                volume.integrate(rgbd_image,
                        cam_intrinsic,
                        np.linalg.inv(pose))
    mesh = volume.extract_triangle_mesh()
    #o3d.visualization.draw_geometries([mesh])
    o3d.io.write_triangle_mesh(output_file, mesh)


import sys
if __name__ == '__main__':
    data_dir = '/Users/bingjian-bj/Downloads/area_3/raw/'
    spots = []
    spots.append('4d491624b8dd4db9999935affb0c4ada')
    spots.append('5c2959c3089c4ac5a7c5c913cc0df78d')
    """
    ss = sorted(glob.glob(os.path.join(data_dir, '*_intrinsics_0.txt')))
    spots = [os.path.basename(s)[:32] for s in ss]
    spots = [s for s in spots if s[0] in ['4', '5', '6']]
    print(len(spots))
    spots = sorted(spots)
    """
    output_file = sys.argv[1]
    main(data_dir, spots, output_file)
