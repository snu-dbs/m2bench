import os
import sys
import customer_and_person
import graph_model
import relational_model
import document_model


def main(argv):
    tpcds_dir_path = argv[0]
    unibench_dir_path = argv[1]

    ## directory check
    outdir_path = '../../ecommerce'
    subdirs=["/array", "/json", "/table"]
    for i in subdirs:
        outdir_path_i = outdir_path+i
        if os.path.exists(outdir_path+i):
            os.system("rm -rf " + outdir_path_i)
        os.mkdir(outdir_path_i)

    if os.path.exists(outdir_path+"/property_graph"):
        for file in os.scandir(outdir_path+"/property_graph"):
            fname, ext = os.path.splitext(file)
            if "../../ecommerce/property_graph/hashtag" not in fname:
                os.remove(file)


    # data generation
    print("------ Ecommerce Data Generator ------")
    print("Generating Customer and Person_node data...")
    customer_and_person.generator(tpcds_dir_path, outdir_path)

    print("Generating graph model data...")
    graph_model.edge_generator(unibench_dir_path, outdir_path)

    print("Generating relational model data...")
    relational_model.product_generator(unibench_dir_path, outdir_path)
    relational_model.brand_generator(unibench_dir_path, outdir_path)

    print("Generating document model data...")
    document_model.order_generator(unibench_dir_path, outdir_path)
    document_model.review_generator(outdir_path)

    print("All dataset generated.")


if __name__ == '__main__':
    main(sys.argv[1:])

