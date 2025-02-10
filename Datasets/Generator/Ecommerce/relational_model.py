import pandas as pd
from pathlib import Path

pd.set_option('mode.chained_assignment',  None)


def product_generator(unibench_dir_path, outdir_path):
    product_dir_path = "Product/"
    outdir_path = outdir_path+"/"

    product = pd.read_csv(unibench_dir_path + product_dir_path + "Product.csv", encoding='ISO-8859-1', sep=",")
    product.columns.values[0] = "product_id"
    product.columns.values[1] = "title"
    product.columns.values[5] = "brand_id"
    product.to_csv(Path(outdir_path+"table/") / "Product.csv", header=True, columns=['product_id', 'title', 'price', 'brand_id'], index=False, sep=',')


def brand_generator(unibench_dir_path, outdir_path):
    brand_dir_path = "Vendor/"
    outdir_path = outdir_path+"/"
    
    brand = pd.read_csv(unibench_dir_path + brand_dir_path + "Vendor.csv", encoding='utf-8', sep=",")
    brand.columns.values[0] = "name"
    brand.columns.values[1] = "country"
    brand.columns.values[2] = "industry"

    brand = brand.reset_index()
    brand['brand_id'] = brand.index
    brand['industry'].loc[: int(len(brand)/3)] = 'Amateur Sports'
    brand['industry'].loc[int(len(brand)/3)+1 : int(len(brand)/3)*2+1] = 'Activewear'
    brand['industry'].loc[int(len(brand)/3)*2+2:] = 'Leisure'
    brand.to_csv(Path(outdir_path+"table/") / "Brand.csv", header=True, columns=['brand_id', 'name', 'country', 'industry'], index=False, sep=',')
