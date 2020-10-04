import argparse
from notebooks import metnum
import pandas as pd

k_value = 4
alpha_value = 27

def run_knn(parameters, X_train, y_train, X_test):
  knn = metnum.KNNClassifier(k_value)
  knn.fit(X_train, y_train)
  y_predict = knn.predict(X_test)

  pd.DataFrame({'ImageId': range(1, y_predict.shape[0]+1), 'Label': y_predict}).to_csv(parameters.output, index=False)

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('-m', '--method', type=int)
  parser.add_argument('-i', '--trainset')
  parser.add_argument('-q', '--testset')
  parser.add_argument('-o', '--output')

  parameters = parser.parse_args()

  df = pd.read_csv(parameters.trainset)
  df_test = pd.read_csv(parameters.testset)

  print("Nombre del dataset: {}".format(parameters.trainset))
  print("Nombre del testset: {}".format(parameters.testset))

  print("Cantidad de documentos: {}".format(df.shape[0]))
  print("Cantidad de documentos a categorizar: {}".format(df_test.shape[0]))

  y_train = df["label"]
  X_train = df.drop(columns=['label'])

  X_test = df_test

  if parameters.method == 0:  # kNN
    print("Metodo utilizado: kNN")
    print("k utilizado {}".format(k_value))
    run_knn(parameters, X_train, y_train, X_test)

  elif parameters.method == 1:  # kNN+PCA
    print("Metodo utilizado: PCA + kNN")
    print("alfa utilizado {}".format(alpha_value))
    print("k utilizado {}".format(k_value))

    clf_pca = metnum.PCA(alpha_value)
    clf_pca.fit(X_train)
    X_train_pca = clf_pca.transform(X_train)
    X_test_pca = clf_pca.transform(X_test)
    run_knn(parameters, X_train_pca, y_train, X_test_pca)
