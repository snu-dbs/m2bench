import csv
import pandas as pd


def product_generator(unibench_dirpath):
    product_dirpath = "Product/"

    product = pd.read_csv(unibench_dirpath + product_dirpath + "Product.csv", encoding='ISO-8859-1', sep=",")
    product.drop(['imgUrl', 'productId'], axis=1, inplace=True)
    product.rename(columns= {"asin": "product_id", "brand": "brand_id"}, inplace=True)
    product.to_csv('Product.csv', header=True, index=False, sep=',')


def brand_generator(unibench_dirpath):
    brand_dirpath = "Vendor/"

    brand = pd.read_csv(unibench_dirpath + brand_dirpath + "Vendor.csv", encoding='ISO-8859-1', sep=",")
    brand.rename(columns= {"id": "name", "Country": "country", "Industry": "industry"}, inplace=True)

    brand = brand.reset_index()
    brand['brand_id'] = brand.index
    brand['industry'].loc[: int(len(brand)/3)] = 'Amateur Sports'
    brand['industry'].loc[int(len(brand)/3)+1 : int(len(brand)/3)*2+1] = 'Activewear'
    brand['industry'].loc[int(len(brand)/3)*2+2:] = 'Leisure'
    brand.to_csv('Brand.csv', header=True, columns=['brand_id', 'name', 'country', 'industry'], index=False, sep=',')